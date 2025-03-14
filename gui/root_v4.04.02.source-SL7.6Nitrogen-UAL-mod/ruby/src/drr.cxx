// @(#)root/ruby:$Name:  $:$Id: drr.cxx,v 1.4 2004/06/03 13:07:19 brun Exp $
// Author:  Elias Athanasopoulos, May 2004

/*  dynamic ruby-root
 *  (http://null.edunet.uoa.gr/~elathan/rr/)
 *  
 *  Elias Athanasopoulos            <elathan@phys.uoa.gr>
 *  George Tzanakos                 <tzanakos@cc.uoa.gr> 
 *
 *  University of Athens 
 *  Department of Physics  
 *  HEPA Lab
 *  (http://daedalus.phys.uoa.gr)
 *
 *  (c) 2003, 2004
*/


#include "TROOT.h"
#include "TApplication.h"
#include "TSystem.h"
#include "TRandom.h"
#include "TF2.h"
#include "TBenchmark.h"
#include "TVirtualPad.h"
#include "TStyle.h"

#include "CallFunc.h"
#include "Class.h"

#include "ruby.h"

#include "rrcommon.h"

/* ROOT's global enums.  */
#include "rrenums.h"

/* Special for Unixes */
#if defined(linux) || defined(sun)
  #include "dlfcn.h"
#endif

VALUE cTObject;

VALUE rr_ary_new (TList *l)
{
    /* convert a TList to a Ruby array */
    VALUE arr = rb_ary_new();
    VALUE o;
    
    TObject *rro;
    TIter next (l);

    while ((rro = next()))
      {
        RRNEW(o, cTObject);
        rb_iv_set (o, "__rr__", Data_Wrap_Struct (cTObject, 0, 0, rro));
        rb_iv_set (o, "__rr_class__",
                rb_str_new2(rro->ClassName()));
        rb_ary_push (arr, o); 
      }
   
    return arr;
}

static VALUE rr_to_ary (VALUE self)
{
    /* convert a TCollection to a Ruby array */
    RRGRAB(self, TList, l);
    return rr_ary_new (l);
}

VALUE rr_arrayc_new (const TArrayC *a)
{
    /* convert a TArrayC to a Ruby array */
    VALUE arr = rb_ary_new();

    for (int i = 0; i < a->GetSize(); i++)
        rb_ary_push (arr, INT2NUM(a->At(i)));        
   
    return arr;
}

VALUE rr_arrays_new (const TArrayS *a)
{
    /* convert a TArrayS to a Ruby array */
    VALUE arr = rb_ary_new();

    for (int i = 0; i < a->GetSize(); i++)
        rb_ary_push (arr, INT2NUM(a->At(i)));        
   
    return arr;
}

VALUE rr_arrayi_new (const TArrayI *a)
{
    /* convert a TArrayI to a Ruby array */
    VALUE arr = rb_ary_new();

    for (int i = 0; i < a->GetSize(); i++)
        rb_ary_push (arr, INT2NUM(a->At(i)));        
   
    return arr;
}

VALUE rr_arrayl_new (const TArrayL *a)
{
    /* convert a TArrayL to a Ruby array */
    VALUE arr = rb_ary_new();

    for (int i = 0; i < a->GetSize(); i++)
        rb_ary_push (arr, INT2NUM(a->At(i)));        
   
    return arr;
}

VALUE rr_arrayf_new (const TArrayF *a)
{
    /* convert a TArrayC to a Ruby array */
    VALUE arr = rb_ary_new();

    for (int i = 0; i < a->GetSize(); i++)
        rb_ary_push (arr, rb_float_new(a->At(i)));        
   
    return arr;
}

VALUE rr_arrayd_new (const TArrayD *a)
{
    /* convert a TArrayD to a Ruby array */
    VALUE arr = rb_ary_new();

    for (int i = 0; i < a->GetSize(); i++)
        rb_ary_push (arr, rb_float_new(a->At(i)));        
   
    return arr;
}

VALUE rr_seqcollection_new (TSeqCollection *sc)
{
    /* convert a TSeqCollection to a Ruby Array */
    VALUE arr = rb_ary_new();
    VALUE o;
    
    for (int i = 0; i < sc->GetSize(); i++)
      {
        RRNEW(o, cTObject);
        rb_iv_set (o, "__rr__", Data_Wrap_Struct (cTObject, 0, 0, sc->At(i)));
        rb_ary_push (arr, o); 
      }
    
    return arr;
}

void * rr_parse_void (VALUE o)
{
    VALUE *i;
    
    switch (TYPE(o))
      {
        case T_STRING:
            return (void *) RSTRING(o)->ptr;
        case T_FLOAT:
            return (void *) &RFLOAT(o)->value;
        case T_FIXNUM:
            /* FIXME: Memory leak until I find the correct way. Until
             * then please use integers in TTrees with care. --elathan 
             */
            i = (VALUE*) malloc (sizeof(int));
            *i = (int) (o>>1);
            return (void *) i;
        case T_OBJECT:
            RRGRAB(o, void *, res);
            return res;
        default:
            rb_fatal ("Failed convertion of %d to void *.\n", CLASS_OF(o));
            break;
      }

    return (void *) NULL;
}

VALUE rr_bool (bool q)
{
    VALUE res = Qnil;
    
    q == 0 ? res = Qfalse : res = Qtrue;

    return res;
}

/* Wrappers for function pointers.  */

/* TF1 */
static struct rr_fcn_info * rr_tf1_table[256];
static int rr_tf1_tblptr = 0;

double rr_ctf1_fcn (double *x, double* par)
{
    TF1 *fcn = (TF1 *)TF1::GetCurrent();
    struct rr_fcn_info *info = NULL;
    
    for (int i = 0; i < rr_tf1_tblptr; i++)
      { 
        info = rr_tf1_table[i];
        if (!strcmp(info->name, fcn->GetName()))
            break;
        else
            info = NULL;
      }

    if (info == NULL)
    rb_warn("Ruby user defined function has not been registered for %s (%p).",
        fcn->GetName(), fcn);

    int n = fcn->GetNpar();
    VALUE vx = rb_ary_new2 (n);
    VALUE vpar = rb_ary_new2 (n);
    for (int i = 0; i < n; i++)
      {
        rb_ary_push (vx, rb_float_new(x[i]));
        rb_ary_push (vpar, rb_float_new(par[i]));
      }

    double res = NUM2DBL(rb_funcall (rb_cObject, info->id, 2, vx, vpar));
    // DEBUG! printf("got: %f\n", res);
    return res;
}

void rr_register_ctf1_fcn (char *name, ID id)
{
    struct rr_fcn_info *info = (struct rr_fcn_info *)malloc (sizeof *info);
    
    info->name = strdup(name);
    info->id = id;
     
    rr_tf1_table[rr_tf1_tblptr] = info;
    rr_tf1_tblptr++;
    
}

static struct rr_fcn_info * rr_tf2_table[256];
static int rr_tf2_tblptr = 0;

double rr_ctf2_fcn (double *x, double* par)
{
    TF2 *fcn = (TF2 *)TF2::GetCurrent();
    struct rr_fcn_info *info = NULL;
    
    for (int i = 0; i < rr_tf2_tblptr; i++)
      { 
        info = rr_tf2_table[i];
        if (!strcmp(info->name, fcn->GetName()))
            break;
        else
            info = NULL;
      }

    if (info == NULL)
    rb_warn("Ruby user defined function has not been registered for %s (%p).",
        fcn->GetName(), fcn);

    int n = fcn->GetNpar();
    VALUE vx = rb_ary_new2 (n);
    VALUE vpar = rb_ary_new2 (n);
    for (int i = 0; i < n; i++)
      {
        rb_ary_push (vx, rb_float_new(x[i]));
        rb_ary_push (vpar, rb_float_new(par[i]));
      }

    double res = NUM2DBL(rb_funcall (rb_cObject, info->id, 2, vx, vpar));
    // DEBUG! printf("got: %f\n", res);
    return res;
}

void rr_register_ctf2_fcn (char *name, ID id)
{
    struct rr_fcn_info *info = (struct rr_fcn_info *)malloc (sizeof *info);
    
    info->name = strdup(name);
    info->id = id;
     
    rr_tf2_table[rr_tf2_tblptr] = info;
    rr_tf2_tblptr++;
    
}
/* Implementation */

/* Globals */

static VALUE rr_gsystem (void)
{
    VALUE o;

    RRNEW(o, cTObject);
    rb_iv_set (o, "__rr__", Data_Wrap_Struct(cTObject, 0, 0, gSystem));
    rb_iv_set (o, "__rr_class__", rb_str_new2("TSystem"));

    return o;
}

static VALUE rr_grandom (void)
{
    VALUE o;

    RRNEW(o, cTObject);
    rb_iv_set (o, "__rr__", Data_Wrap_Struct(cTObject, 0, 0, gRandom));
    rb_iv_set (o, "__rr_class__", rb_str_new2("TRandom"));
    
    return o;
}

static VALUE rr_gbenchmark (void)
{
    VALUE o;
    
    RRNEW(o, cTObject);
    rb_iv_set (o, "__rr__", Data_Wrap_Struct(cTObject, 0, 0, gBenchmark));
    rb_iv_set (o, "__rr_class__", rb_str_new2("TBenchmark"));
    
    return o;
}

static VALUE rr_gpad (void)
{
    VALUE o;
    
    RRNEW(o, cTObject);
    rb_iv_set (o, "__rr__", Data_Wrap_Struct(cTObject, 0, 0, gPad));
    rb_iv_set (o, "__rr_class__", rb_str_new2("TPad"));
    
    return o;
}

static VALUE rr_gstyle (void)
{
    VALUE o;
    
    RRNEW(o, cTObject);
    rb_iv_set (o, "__rr__", Data_Wrap_Struct(cTObject, 0, 0, gStyle));
    rb_iv_set (o, "__rr_class__", rb_str_new2("TStyle"));
    
    return o;
}

static VALUE rr_gdirectory (void)
{
    VALUE o;
    
    RRNEW(o, cTObject);
    rb_iv_set (o, "__rr__", Data_Wrap_Struct(cTObject, 0, 0, gDirectory));
    rb_iv_set (o, "__rr_class__", rb_str_new2("TDirectory"));
    
    return o;
}

static VALUE rr_groot (void)
{
    VALUE o;

    RRNEW(o, cTObject);

    rb_iv_set (o, "__rr__", Data_Wrap_Struct(cTObject, 0, 0, gROOT));
    rb_iv_set (o, "__rr_class__", rb_str_new2("TROOT"));
   
    return o;
}

static VALUE rr_gapplication (void)
{
    VALUE o;

    RRNEW(o, cTObject);

    rb_iv_set (o, "__rr__", Data_Wrap_Struct(cTObject, 0, 0, gApplication));
    rb_iv_set (o, "__rr_class__", rb_str_new2("TApplication"));
   
    return o;
}

static VALUE via (VALUE self, VALUE ameth, VALUE bmeth, VALUE parms)
{
    if (TYPE(ameth) != T_SYMBOL &&
        TYPE(bmeth) != T_SYMBOL &&
        TYPE(parms) != T_HASH)
      {
        rb_fatal ("rr-via: Please call TObject#via with sym, sym, hash.");
        return Qnil;
      }
    
    VALUE keys = rb_funcall(parms, rb_intern("keys"), 0);
    for (int i = 0; i < RARRAY(keys)->len; i++)
      {
        VALUE key = rb_ary_entry (keys, i);               
        rb_funcall (self, rb_to_id (ameth), 2, key, rb_hash_aref (parms, key));
      }
    rb_funcall(self, rb_to_id(bmeth), 0);

    return self;
}

/* Dynamic ruby-root specific implementation.  */

TObject* drr_grab_object(VALUE self)
{
    static TObject *o;
    Data_Get_Struct(rb_iv_get (self, "__rr__"), TObject, o);
    return o;
}

void drr_map_args2(VALUE inargs, char *cproto, G__CallFunc *f, int offset=1)
{
    /* FIXME. Offset reminds me fortran code; make a better interface,
     * and change the function name to a better one. 
     * 
     * The boolean checks for cproto and f are vital. This function can
     * be called:
     *
     * 1. When we want a C prototype from a Ruby call
     * 2. When we want to set the arguments of a CINT function
     * 3. When we want both 1 and 2
     */
        
    int nargs = RARRAY(inargs)->len - offset;
    double *arr = NULL;
    TObject *ptr = NULL;
    VALUE v = 0;
    
    /* Transform Ruby arguments to C/C++.  */
    for (int i = 0; i < nargs; i++)
      {
        VALUE arg = rb_ary_entry (inargs, i+offset);
        switch (TYPE(arg))
          {
            case T_FIXNUM:
                if (f) f->SetArg((long) NUM2INT(arg));
                if (cproto) strcat(cproto, "int");
                break;
            case T_FLOAT:
                if (f) f->SetArg(NUM2DBL(arg));
                if (cproto) strcat(cproto, "double");
                break;
            case T_STRING:
                if (f) f->SetArg((long) STR2CSTR(arg));
                if (cproto) strcat(cproto, "char*");
                break;
            case T_ARRAY:
                /* FIXME: Handle all arrays, not only
                 * with floats.
                 */
                if (f)
                  {
                    arr = ALLOC_N (double, RARRAY(arg)->len);
                    for (int j = 0; j < RARRAY(arg)->len; j++)
                        arr[j] = NUM2DBL(rb_ary_entry (arg, j));
                    f->SetArg((long) arr);
                  }
                if (cproto) strcat(cproto, "double*"); 
                break;
            case T_OBJECT:
                v = rb_iv_get (arg, "__rr__");
                if (!NIL_P(v))
                  {
                    Data_Get_Struct (v, TObject, ptr);
                    if (f) f->SetArg((long) ptr);
                    if (cproto) {
                        strcat(cproto, STR2CSTR(rb_iv_get (arg, "__rr_class__")));
                        strcat(cproto, "*");
                    }
                  }
                break;
            default:
                break;
          }
        if ((i + 1 < nargs) && (nargs != 1) && cproto)
            strcat(cproto, ",");
      }
}

/* FIXME. Err... enum is the correct word? :-) */
#define kint        0 
#define kfloat      1
#define kchar       2
#define kunknown    3
#define kvoid       4
#define kintary     5
#define kfloatary   6
#define kstring     7
#define kroot       8
#define kbool       9

int drr_parse_ret_type (const char *ret)
{
    /* 
        0: int
        1: float
        2: char
        3: Unknown
        4: void
        5: Array of ints
        6: Array of floats
        7: String
        8: ROOT Object 
    */

    char *realtype = strdup(ret), *t = realtype;
    int plevel = 0, type;

    while (*(t++)) {
        if (*t == '*')
            plevel++;
    }   

    t--;

    if (plevel > 0)
        *(t - plevel) = '\0';

    if (!strncmp(t - 3, "int", 3) ||
        !strncmp(t - 4, "long", 4))
        type = kint;
    else
    if (!strncmp(t - 6, "double", 6) ||
        !strncmp(t - 5, "float", 5))       
        type = kfloat;
    else
    if (!strncmp(t - 5, "char", 4))  
        type = kchar;
    else
    if (!strncmp(t - 4, "void", 4))
        type = kvoid;
    else
    if (!strncmp(t - 4, "bool", 4))
        type = kbool;
    else
        type = kunknown;

    if (plevel)
        type += 5;

    free (realtype);

    return type;
}

/* Function cache related.  */

struct drr_func_cache * drr_func_cache_init(struct drr_func_entry *entry)
{
    struct drr_func_cache *new_cache = (struct drr_func_cache *) malloc (sizeof *new_cache);
    new_cache->next = NULL;
    new_cache->entry = entry;
    new_cache->last = NULL; 
    return new_cache;
}

void drr_func_cache_push (struct drr_func_cache *cache, struct drr_func_entry *entry)
{
    struct drr_func_cache *n = (struct drr_func_cache *) malloc(sizeof *n);
    n->entry = entry;
    
    if (cache->next)
      {
        n->next = cache->next;
        cache->next = n; 
      }
    else
      {
        cache->next = n;
        n->next = NULL;
      }
}

struct drr_func_entry * drr_func_cache_find (struct drr_func_cache *cache, char *name)
{
    struct drr_func_cache *iter = cache;

    while (iter) 
      {
        if (!strcmp (iter->entry->name, name))
            return iter->entry;
        iter = iter->next;
      }
    return NULL;
}

void drr_func_entry_free (struct drr_func_entry *entry)
{
    delete entry->func;
    delete entry->klass;
    free (entry->name);
    free (entry->cproto);
    free (entry);
}

/* Ruby generic interface.  */

VALUE drrAbstractClass;

static VALUE drr_as(VALUE self, VALUE klass)
{
    /* Pseudo C++ casting.  */
    VALUE v;
    
    /* Check if there is a ROOT dict. available.  */
    TClass *c = new TClass(STR2CSTR(klass));
    if (c)
      {
        VALUE k;
        char *name = STR2CSTR(klass);
        if (!rb_const_defined (rb_cObject, rb_intern(name)))
            k = rb_define_class (name, drrAbstractClass);            
        else
            k = rb_path2class (name);
        
        RRNEW(v, k);
        rb_iv_set (v, "__rr__", rb_iv_get(self, "__rr__"));
        rb_iv_set (v, "__rr_class__", klass);
      }
    else
        rb_fatal ("Cast to %s is not allowed. Aborting.", STR2CSTR(klass));
    
    delete c;
    return v;
}

static VALUE drr_init(int argc, VALUE argv[], VALUE self)
{
    VALUE inargs;
    char *classname = rb_obj_classname(self);
    char cproto[1024] = "";
    long addr = 0, offset;
   
    rb_scan_args (argc, argv, "0*", &inargs);
    
    //rb_warn("You tried to call: %s#%s with %d arguments", classname, classname, RARRAY(inargs)->len);
 
    G__CallFunc func;
    G__ClassInfo klass(classname);

    /* Call the requested ctor.  */
  
    if (RARRAY(inargs)->len)
        drr_map_args2 (inargs, cproto, &func, 0);

    G__MethodInfo minfo(klass.GetMethod(classname, cproto, &offset)); 
    if (minfo.InterfaceMethod())
        func.SetFunc(minfo);
    else
        rb_fatal("You provided a wrong prototype (%s) for (%s#%s). Aborting.", 
                    cproto, classname, classname);

    addr = func.ExecInt((void*)((long)NULL + offset));
    rb_iv_set(self, "__rr__", Data_Wrap_Struct(cTObject, 0, 0, (TObject *)addr));
    rb_iv_set(self, "__rr_class__", rb_str_new2 (classname));

    func.Init();
    return self;
}

static VALUE drr_const_missing(VALUE self, VALUE klass)
{
    /* Define a new ROOT Class dynammically.  */

    /* Silence a silly gcc warning...  */
    if (NIL_P(self))
        return Qnil;
    
    char *name = rb_id2name (rb_to_id(klass));
   
    /* Check if there is a ROOT dict. available.  */
    TClass *c = new TClass(name);
    if (c)
      {
        VALUE new_klass = rb_define_class (name, drrAbstractClass);
        delete c;
        return new_klass;
      }
    else
        /* FIXME: raise exception here.  */
        rb_warn ("Constant %s is not defined.", name);
    
    delete c;
    return Qnil;
}

static VALUE drr_method_missing(int argc, VALUE argv[], VALUE self)
{
    /* When a ROOT method is called, we try to resolve it here. If
     * CINT is able to resolve it then we define a Ruby method using
     * a similar generic function (drr_generic_method), so as
     * Ruby not to use the Object#method_missing every time. 
     */
        
    VALUE inargs;
    char *methname, *classname ;
    long offset, address = 0;
    double dbladdr = 0;
    char cproto[1024] = "";
    int nargs;

    /* Grab method, class and the instance pointer.  */
    methname = rb_id2name (rb_to_id(argv[0]));
    classname = STR2CSTR(rb_iv_get (self, "__rr_class__"));
    TObject *caller = drr_grab_object (self);
   
    rb_scan_args (argc, argv, "0*", &inargs);

    nargs = RARRAY(inargs)->len - 1;
    VALUE rklass = rb_class_of (self);

    // rb_warn("(mm) - You tried to call: %s#%s with %d arguments", classname, methname, nargs);
    
    G__CallFunc *func = new G__CallFunc();
    G__ClassInfo *klass = new G__ClassInfo (classname);

    if (nargs)
        drr_map_args2 (inargs, cproto, func, 1);

    /* FIXME: minfo is really used only for the return type.  */ 
    G__MethodInfo *minfo = new G__MethodInfo(klass->GetMethod(methname, cproto, &offset)); 
    if (minfo->InterfaceMethod())
        func->SetFunc(*minfo);
    else
        rb_fatal("You provided a wrong prototype (%s) for %s#%s. Aborting.", 
                        cproto, classname, methname);

    /* This is the first time this method is called. Create a cash entry.  */
    struct drr_func_entry *entry = (struct drr_func_entry *) malloc (sizeof *entry);
    entry->func = func;
    entry->klass = klass;
    entry->name = strdup(methname);
    entry->cproto = strdup(cproto);
    entry->rtype = drr_parse_ret_type (minfo->Type()->TrueName());
    
    delete minfo;

    struct drr_func_cache *cache;
    /* If there is no cache available, create one (per Class scope).  */
    if (!rb_cvar_defined (rklass, rb_intern("@@__func_table__"))) 
        cache = drr_func_cache_init (entry);
    else
        Data_Get_Struct(rb_cv_get(rklass, "@@__func_table__"), struct drr_func_cache, cache);

    /* Push the method to the cache and save it back to the Class.  */
    drr_func_cache_push (cache, entry);
    rb_cv_set (rklass, "@@__func_table__", 
        Data_Wrap_Struct(cTObject, 0, 0, cache));  
      
    if (entry->rtype != kfloat)
        address = func->ExecInt((void*)((long)caller + offset));
    else
        dbladdr = func->ExecDouble((void*)((long)caller + offset));

    /* Define method.  */
    rb_define_method (rklass, methname, VALUEFUNC(drr_generic_method), -1);

    VALUE vret;

    switch (entry->rtype)
      {
        case kint:
            vret = INT2NUM(address);
            break;
        case kfloat:
            vret = rb_float_new(dbladdr);
            break;
        case kstring:
            vret = rb_str_new2((char *)address);
            break;
        case kbool:
            vret = rr_bool((bool)address);
            break;
        case kroot:
            if (!address)
                return Qnil;

            if (!strcmp(((TObject*)(address))->ClassName(), "TList"))
                vret = rr_ary_new((TList*)address);
            else 
              {
                VALUE res;
                RRNEW(res, cTObject);
                rb_iv_set(res, "__rr__", Data_Wrap_Struct(cTObject, 0, 0, (TObject*)address));
                rb_iv_set(res, "__rr_class__", rb_str_new2 (((TObject*)(address))->ClassName()));
                vret = res;
              }
            break;

        default:
            vret = self;
            break;
      }

    return vret;
}

static VALUE drr_generic_method(int argc, VALUE argv[], VALUE self)
{
    VALUE inargs;
    VALUE rklass;
    int nargs;
    long offset = 0, address = 0;
    double dbladdr = 0;
    char cproto[1024] = "";
    
    /* Grab class, method name and instance pointer.  */
    rklass = rb_class_of (self);
    char *methname = rb_id2name (rb_frame_last_func());
    TObject *caller = drr_grab_object (self);
    
    rb_scan_args (argc, argv, "0*", &inargs);

    nargs = RARRAY(inargs)->len;
   
    // rb_warn("(gm) - You tried to call: %s#%s with %d arguments", rb_obj_classname (self), methname, nargs);
    
    G__CallFunc *func = NULL; 
    
    struct drr_func_cache *cache;
    struct drr_func_entry *entry;

    Data_Get_Struct (rb_cv_get(rklass, "@@__func_table__"), struct drr_func_cache, cache);
    entry = drr_func_cache_find (cache, methname);

    if (entry)
      {
        func = entry->func;
        if (nargs)
            drr_map_args2 (inargs, cproto, NULL, 0);
        func->SetFuncProto (entry->klass, methname, cproto, &offset);
        /* FIXME: Why on earth CINT resets the arguments when
         * SetFuncProto() is called?
         */
        if (nargs)
            drr_map_args2 (inargs, NULL, func, 0);
      }
    else
        /* FIXME: This can never be happened.  */
        rb_warn ("Proto conflict with cache. Expected %s, but found: %s", cproto, entry->cproto);

    if (entry->rtype != kfloat)
        address = func->ExecInt((void*)((long)caller + offset));
    else
        dbladdr = func->ExecDouble((void*)((long)caller + offset));

    VALUE vret;
    switch (entry->rtype)
      {
        case kint:
            vret = INT2NUM(address);
            break;
        case kfloat:
            vret = rb_float_new(dbladdr);
            break;
        case kstring:
            vret = rb_str_new2((char *)address);
            break;
        case kbool:
            vret = rr_bool((bool)address);
            break;
        case kroot:
            if (!address)
                return Qnil;

            if (!strcmp(((TObject*)(address))->ClassName(), "TList"))
                vret = rr_ary_new((TList*)address);
            else
              {
                VALUE res;
                RRNEW(res, cTObject);
                rb_iv_set(res, "__rr__", Data_Wrap_Struct(cTObject, 0, 0, (TObject*)address));
                rb_iv_set(res, "__rr_class__", rb_str_new2 (((TObject*)(address))->ClassName()));
                vret = res;
              }
            break;

        default:
            vret = self;
            break;
      }

    return vret;
}

extern "C"
void Init_libRuby() {

    /* Create a new ROOT Application if it doesn't already exist.  */
    if (!gApplication)
    gApplication = new TApplication("ruby root app", NULL, NULL);

    /* In order to have the most frequently used dictionaries
     * loaded by default. THIS MUST BE REPLACED BY PORTABLE CODE  */
#if defined(linux) || defined(sun)
   dlopen( "libCint.so",   RTLD_GLOBAL | RTLD_LAZY );
   dlopen( "libCore.so",   RTLD_GLOBAL | RTLD_LAZY );
   dlopen( "libGpad.so",   RTLD_GLOBAL | RTLD_LAZY );
   dlopen( "libGraf.so",   RTLD_GLOBAL | RTLD_LAZY );
   dlopen( "libMatrix.so", RTLD_GLOBAL | RTLD_LAZY );
   dlopen( "libHist.so",   RTLD_GLOBAL | RTLD_LAZY );
   dlopen( "libTree.so",   RTLD_GLOBAL | RTLD_LAZY );
   dlopen( "libGraf3d.so", RTLD_GLOBAL | RTLD_LAZY );
   dlopen( "libGeom.so",   RTLD_GLOBAL | RTLD_LAZY );
#endif
   
    drrAbstractClass = rb_define_class("DRRAbstractClass", rb_cObject);
    rb_define_method(drrAbstractClass, "initialize", VALUEFUNC(drr_init), -1);
    rb_define_method(drrAbstractClass, "method_missing", VALUEFUNC(drr_method_missing), -1);
    rb_define_method (drrAbstractClass, "as", VALUEFUNC(drr_as), 1);
    
    cTObject = rb_define_class("TObject", drrAbstractClass);

    rb_define_method (cTObject, "to_ary", VALUEFUNC(rr_to_ary), 0);
    rb_define_method (rb_cObject, "via", VALUEFUNC(via), 3);
    rb_define_singleton_method (rb_cObject, "const_missing", VALUEFUNC(drr_const_missing), 1);

    /* usefull globals */
    rb_define_method (rb_cObject, "gSystem", VALUEFUNC(rr_gsystem), 0);
    rb_define_method (rb_cObject, "gRandom", VALUEFUNC(rr_grandom), 0);
    rb_define_method (rb_cObject, "gBenchmark", VALUEFUNC(rr_gbenchmark), 0);
    rb_define_method (rb_cObject, "gPad", VALUEFUNC(rr_gpad), 0);
    rb_define_method (rb_cObject, "gStyle", VALUEFUNC(rr_gstyle), 0);
    rb_define_method (rb_cObject, "gDirectory", VALUEFUNC(rr_gdirectory), 0);
    rb_define_method (rb_cObject, "gROOT", VALUEFUNC(rr_groot), 0);
    rb_define_method (rb_cObject, "gApplication", VALUEFUNC(rr_gapplication), 0);

    /* enums */
    init_global_enums();
}
