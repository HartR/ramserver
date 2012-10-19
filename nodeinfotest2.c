#include <libgimp/gimp.h>    //GIMP plugin stuff
#include <libgimp/gimpui.h>    //GIMP plugin stuff for gtk module
#include <gio/gio.h>        // For all the DBus communcation
#include <stdlib.h>        
#include <stdio.h>      
#include <stdarg.h>
#include <string.h>

// +------------------------------------------------------------------------------
// | Support for Printf-Style Debugging |
// +------------------------------------+

/**
 * When you want messages, use the first version of #define DEBUG.  When you
 * want to turn them off, use the second.
 */

#define DEBUG(...) do { fprintf (stderr, __VA_ARGS__); fprintf (stderr, "\n"); } while (0)

// #define DEBUG(...) do { } while (0)


struct gimpnames {
  gchar **procnames;
  gint nprocs;
};


static GDBusNodeInfo *introspection_data = NULL;
static void
handle_method_call (GDBusConnection       *connection,
                    const gchar           *sender,
                    const gchar           *object_path,
                    const gchar           *interface_name,
                    const gchar           *method_name,
                    GVariant              *parameters,
                    GDBusMethodInvocation *invocation,
                    gpointer               user_data)
{
  //get method nparams, and nreturn_vals
  gchar           *proc_blurb;
  gchar           *proc_help;
  gchar           *proc_author;
  gchar           *proc_copyright;
  gchar           *proc_date;
  GimpPDBProcType  proc_type;
  GimpParamDef    *formals;       
  gint             nparams;
  GimpParamDef    *return_types;
  gint             nreturn_vals;
 
  if (! gimp_procedural_db_proc_info (method_name,
                                      &proc_blurb,
                                      &proc_help,
                                      &proc_author,
                                      &proc_copyright,
                                      &proc_date,
                                      &proc_type,
                                      &nparams, &nreturn_vals,
                                      &formals, &return_types))
    {
      printf("Invalid Method Name");
    }

  //parse GVariant parameters and convert to GimpParam

GimpParam *prms = g_try_malloc (nparams * sizeof (GimpParam));
/* int i;
 for (i=0; i<nparam; i++)
   {
   g_variant_get_type_string(parameters[i])*/







}//handle_method_call
static GVariant *
handle_get_property (GDBusConnection  *connection,
                     const gchar      *sender,
                     const gchar      *object_path,
                     const gchar      *interface_name,
                     const gchar      *property_name,
                     GError          **error,
                     gpointer          user_data)
{
  g_set_error (error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND,
               "No property %s found in interface %s.",
	       property_name, 
	       interface_name);
  // NULL signals failure.
  return NULL;
}//get property

static gboolean
handle_set_property (GDBusConnection  *connection,
                     const gchar      *sender,
                     const gchar      *object_path,
                     const gchar      *interface_name,
                     const gchar      *property_name,
                     GVariant         *value,
                     GError          **error,
                     gpointer          user_data)
{
  g_set_error (error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND,
               "No property %s found in interface %s.",
	       property_name, 
	       interface_name);
  // false signals failure.
  return FALSE;
}//set property
static void query (void);
static void run   (const gchar      *name,
                   gint              nparams,
                   const GimpParam  *param,
                   gint             *nreturn_vals,
                   GimpParam       **return_vals);

static const GDBusInterfaceVTable interface_vtable =
  {
    handle_method_call,
    handle_get_property,
    handle_set_property
  };

static void
on_bus_acquired (GDBusConnection *connection,
                 const gchar     *name,
                 gpointer         user_data)
{
  guint registration_id;

  DEBUG("Bus acquired.");

  registration_id = g_dbus_connection_register_object (connection,
                                                       "/edu/grinnell/cs/glimmer/HARTRULES",
                                                       introspection_data->interfaces[0],
                                                       &interface_vtable,
                                                       NULL,  /* user_data */
                                                       NULL,  /* user_data_free_func */
                                                       NULL); /* GERROR */

  DEBUG ("Object registered.");
} // on_bus_acquired

static void
on_name_acquired (GDBusConnection *connection,
                  const gchar     *name,
                  gpointer         user_data)
{
  DEBUG ("Acquired name %s", name);
} // on_name_acquired

static void
on_name_lost (GDBusConnection *connection,
              const gchar     *name,
              gpointer         user_data)
{
  DEBUG ("Lost name %s", name);
  exit (1);
} // on_name_lost


GimpPlugInInfo PLUG_IN_INFO =
  {
    NULL,
    NULL,
    query,
    run
  };

MAIN()

static void
query (void)
{

  // saving in constant memory what the data I expect from Gimp
  static GimpParamDef args[] =
    {
      {

	GIMP_PDB_INT32, // determines the run-mode whether it is interactive or non-interactive
	"run-mode",
	"RM"
      },
      {
	GIMP_PDB_IMAGE, // Input image
	"image",
	"Input image"
      },
      {
	GIMP_PDB_DRAWABLE,// Input drawable
	"drawable",
	"Input drawable"
      }
    };

  gimp_install_procedure (
			  "nodeinfotest2",
			  "Node Info Test",
			  "Publishes Node on the DBUS",
			  "Hart Russell",
			  "Copyright RAMS 2012",
			  "2012",
			  "<Toolbox>/Xtns/MediaScript/NodeInfoTest2",
			  NULL, // was "RGB*, GRAY*",
			  GIMP_PLUGIN,
			  G_N_ELEMENTS (args), 0,
			  args, NULL);
} // query


gpointer *
ap_alt (int count, ...)
{
  int i;
  va_list args;                                                                                                                                               
  gpointer *result = g_new (gpointer, count+1);
  if (result == NULL)
    return NULL;
 
  // Copy the arguments
  va_start (args, count);
  for (i = 0; i < count; i++)
    {
      gpointer p = va_arg (args, void *);
      result[i] = p;
    } //for                                                          
  va_end (args);

  // Null terminate
  result[count] = NULL;

  // And we're done
  return result;
} // new_pointer_array

/**
 * Create a NULL terminated array of n pointers.  Requires that the remaining arguments
 * are pointers, and that there are the approrpiate number.
 */
gpointer *
ap (int count, ...)
{
  int i;
  va_list args;                                                                                                                                               
  gpointer *result = g_new (gpointer, count+1);
  if (result == NULL)
    return NULL;
 
  // Copy the arguments
  va_start (args, count);
  for (i = 0; i < count; i++)
    {
      result[i] = (gpointer) va_arg (args, gpointer);
    } // for                                                          
  va_end (args);

  // Null terminate
  result[count] = NULL;

  // And we're done
  return result;
} // ap

//Node to register on DBUS
GDBusNodeInfo *
node_new (gchar *path,
          GDBusInterfaceInfo **interfaces,
          GDBusNodeInfo **nodes,
          GDBusAnnotationInfo **annotations)
{
  GDBusNodeInfo *node = g_try_malloc (sizeof (GDBusNodeInfo));
  if (node == NULL)
    return NULL;
  node->ref_count = 1;
  node->path = path;
  node->interfaces = interfaces;
  node->nodes = nodes;
  node->annotations = annotations;
  return node;
} // node_new

//main part of gdbusnodeinfo, should utilize helper functions
GDBusInterfaceInfo *
interface_new (gchar    *name, 
               GDBusMethodInfo **methods,
               GDBusSignalInfo **signals,
               GDBusPropertyInfo **properties,
               GDBusAnnotationInfo **annotations)
{
  GDBusInterfaceInfo *interface = g_try_malloc (sizeof (GDBusInterfaceInfo));
  if (interface == NULL)
    return NULL;

  interface->ref_count = 1;
  interface->name = name;
  interface->methods = methods;
  interface->signals = signals;
  interface->properties = properties;
  interface->annotations = annotations;

  return interface;
} // interface_new

//used to help build methodinfo
GDBusArgInfo *
arg_new (gchar *name,
	 const gchar *signature,
         GDBusAnnotationInfo **annotations)
{
  GDBusArgInfo *arg= g_try_malloc (sizeof (GDBusArgInfo));
  if (arg == NULL)
    return NULL;
  
  arg->ref_count = 0;
  arg->name = name;
  arg->signature = signature;
  arg->annotations = annotations;
  
  return arg;
} // arg_new


/**
 * Create a GDBusMethodInfo for GDBusInterfaceInfo.  Returns NULL upon failure.
 */
GDBusMethodInfo *
g_dbus_method_info_build (gchar *name,
			  GDBusArgInfo **in_args, 
			  GDBusArgInfo **out_args,
			  GDBusAnnotationInfo **annotations)
{
  GDBusMethodInfo *method = g_try_malloc (sizeof (GDBusMethodInfo));
  if (method == NULL)
    return NULL;
  method->name = name;
  method->ref_count = 1;
  method->in_args = in_args;
  method->out_args = out_args;
  method->annotations = annotations;
  return method;
} // g_dbus_method_info_build

/**
 * Create a GDBusSignalInfo for GDBusInterfaceInfo.  Returns NULL upon failure.
 */
GDBusSignalInfo *
g_dbus_signal_info_build (gchar *name,
			  GDBusArgInfo **args, 
			  GDBusAnnotationInfo **annotations)
{
  GDBusSignalInfo *signal = g_try_malloc (sizeof (GDBusSignalInfo));
  if (signal == NULL)
    return NULL;
  signal->name = name;
  signal->ref_count = 1;
  signal->args = args;
  signal->annotations = annotations;
  return signal;
} // g_dbus_signal_info_build

/**
 * Create a GDBusPropertyInfo for GDBusInterfaceInfo.  Returns NULL upon failure.
 */
GDBusPropertyInfo *
g_dbus_property_info_build (gchar *name,
			    gchar *signature,
			    GDBusPropertyInfoFlags flags,
			    GDBusAnnotationInfo **annotations)
{
  GDBusPropertyInfo *property = g_try_malloc (sizeof (GDBusPropertyInfo));
  if (property == NULL)
    return NULL;
  property->name = name;
  property->ref_count = 1;
  property->signature = signature;
  property->flags = flags;
  property->annotations = annotations;
  return property;
} // g_dbus_property_info_build

/**
 * Create a GDBusAnnotationInfo for GDBusInterfaceInfo.  Returns NULL upon failure.
 */
GDBusAnnotationInfo *
g_dbus_annotation_info_build (gchar *key,
                              gchar *value,
                              GDBusAnnotationInfo **annotations)
{
  GDBusAnnotationInfo *annotation = g_try_malloc (sizeof (GDBusAnnotationInfo));
  if (annotation == NULL)
    return NULL;
  annotation->ref_count = 1;
  annotation->key = key;
  annotation->value = value;
  annotation->annotations = annotations;
  return annotation;
} // g_dbus_annotation_info_build



/**
 * Convert a GimpParamDef to a GVariant signature
 */
static const gchar *
rserver_pdb_type_to_signature (GimpPDBArgType type)
{
  

  const gchar *result;

  switch (type)
    {
    case GIMP_PDB_INT32:
      result = (const gchar *) G_VARIANT_TYPE_INT32;
      break;
    case GIMP_PDB_INT16:
      result = (const gchar *) G_VARIANT_TYPE_INT16;
      break;
    case GIMP_PDB_INT8:
      result = (const gchar *) G_VARIANT_TYPE_BYTE;
      break;
    case GIMP_PDB_FLOAT:
      result = (const gchar *) G_VARIANT_TYPE_DOUBLE;
      break;
    case GIMP_PDB_STRING:
      result = (const gchar *) G_VARIANT_TYPE_STRING;
      break;

    case GIMP_PDB_DISPLAY:
      result = (const gchar *) G_VARIANT_TYPE_INT32;
      break;
    case GIMP_PDB_IMAGE:
      result = (const gchar *) G_VARIANT_TYPE_INT32;
      break;
    case GIMP_PDB_LAYER:
      result = (const gchar *) G_VARIANT_TYPE_INT32;
      break;

    default:
      result = (const gchar *) G_VARIANT_TYPE_INT32;
      break;
    } // switch

  
  return result;
} // rserver_pdb_param_to_signature


/**
 * Convert a GimpParamDef to a GDBusArgInfo.
 */
GDBusArgInfo *
rserver_pdb_param_to_arginfo (GimpParamDef param)
{
 
  gchar *name = param.name;
  const gchar *type = rserver_pdb_type_to_signature (param.type);
  GDBusArgInfo *result = arg_new (name, type, NULL);
 
  return result;
} // rserver_pdb_param_to_arginfo

/*
  and then use
  http://developer.gimp.org/api/2.0/libgimp/libgimp-gimpproceduraldb.html#gimp-procedural-db-query
  to get a list of stuff
  as in 
  https://github.com/tiwareep/BlahTesting/blob/master/DBUS-Original/Gimp/glist.c
  then use the below to get stuff
  then handle a method call as in rserver using 
  gimp_run_procedure2
  found at
  http://developer.gimp.org/api/2.0/libgimp/libgimp-gimp.html#gimp-run-procedure2
*/


struct gimpnames *
procnamesbuilder ()
{
  struct gimpnames *gimpnamelist;
  gimpnamelist =  g_try_malloc (sizeof (struct gimpnames));
  gimp_procedural_db_query (".*", ".*", ".*", ".*", ".*", ".*", ".*", &gimpnamelist->nprocs, &gimpnamelist->procnames);
  return gimpnamelist;
}

static GDBusMethodInfo *
pdb_method_test (gchar *proc_name)
{
 
  // Lots and lots and lots of fields for getting info.
  gchar           *proc_blurb;
  gchar           *proc_help;
  gchar           *proc_author;
  gchar           *proc_copyright;
  gchar           *proc_date;
  GimpPDBProcType  proc_type;
  GimpParamDef    *formals;       
  gint             nparams;
  GimpParamDef    *return_types;
  gint             nreturn_vals;

  // Parts of our return information
  gint i;                        // Counter variable
  GDBusArgInfo   **args = NULL;         // Argument info
  GDBusArgInfo   **returns = NULL;      // Return value info

  // Get the information 
 
  if (! gimp_procedural_db_proc_info (proc_name,
                                      &proc_blurb,
                                      &proc_help,
                                      &proc_author,
                                      &proc_copyright,
                                      &proc_date,
                                      &proc_type,
                                      &nparams, &nreturn_vals,
                                      &formals, &return_types))
    {
      printf("ya messed up ya dingus");
      return NULL;
    }
 

  // Process the parameters
  if (nparams > 0)
    {
      args = g_new (GDBusArgInfo *, nparams+1);
      for (i = 0; i < nparams; i++)
        {
         
          args[i] = rserver_pdb_param_to_arginfo (formals[i]);
          g_assert (args[i] != NULL);
        } // for
      // Terminate the array
      args[nparams] = NULL;
    } // if (nparams > 0)

  // Process the return values
  if (nreturn_vals > 0)
    {
      returns = g_new (GDBusArgInfo *, nreturn_vals);
      for (i = 0; i < nreturn_vals ; i++)
        {
         
          returns[i] = rserver_pdb_param_to_arginfo (return_types[i]);
          g_assert (returns[i] != NULL);
        } // for
      // Terminate the array
      returns[nreturn_vals] = NULL;
    } // if (nreturn_vals > 0)

  GDBusMethodInfo * result =
    g_dbus_method_info_build (proc_name,
			      args,
			      returns,
			      NULL);

  
  return result;
} // rserver_pdb_method


GDBusMethodInfo **methmaker(struct gimpnames *nms){
  GDBusMethodInfo **nfo =  g_try_malloc (nms->nprocs * sizeof (GDBusMethodInfo *));
  int i;
  for (i = 0; i < nms->nprocs; i++)
    {
      nfo[i] = pdb_method_test(nms->procnames[i]);
    }
  nfo[nms->nprocs]=NULL;
  return nfo;
}



static void
run (const gchar      *name,
     gint              nparams,
     const GimpParam  *param,
     gint             *nreturn_vals,
     GimpParam       **return_vals)
{
  static GimpParam  values[1];
  GimpPDBStatusType status = GIMP_PDB_SUCCESS;
  GimpRunMode       run_mode;

  /* Setting mandatory output values */
  *nreturn_vals = 1;
  *return_vals  = values;

  values[0].type = GIMP_PDB_STATUS;
  values[0].data.d_status = status;
  guint owner_id;


  GString *xml = g_string_new ("");

  GDBusInterfaceInfo **interfaces = g_try_malloc (2 * sizeof (GDBusInterfaceInfo *));
  GDBusInterfaceInfo *interface = g_try_malloc (sizeof (GDBusInterfaceInfo));

  interface->ref_count = 1;
  interface->name = "edu.grinnell.Glimmer.Test01";


  struct gimpnames *gnames;


  GDBusMethodInfo **info;

  DEBUG ("About to create gnames");
  gnames = procnamesbuilder();
  DEBUG ("About to make methods");
  info = methmaker(gnames);
  DEBUG ("Done making methods");

  interface->methods = info;
  interface->signals = NULL;
  interface->properties = NULL;
  interface->annotations = NULL;

  interfaces[0] = interface;
  interfaces[1] = NULL;


  GMainLoop *loop;

  g_type_init ();
 

  DEBUG ("About to make node.");
  GDBusNodeInfo *finalnode = node_new(NULL, interfaces, NULL, NULL);
  DEBUG ("Made node.");

  DEBUG ("About to own name");
  owner_id = g_bus_own_name (G_BUS_TYPE_SESSION,
                             "edu.grinnell.cs.glimmer.RAMStest",
                             G_BUS_NAME_OWNER_FLAGS_NONE,
                             on_bus_acquired,
                             on_name_acquired,
                             on_name_lost,
                             NULL,
                             NULL);
  DEBUG ("Owned name");

  // Event loop.  Wait for functions to get called asyn h.
  loop = g_main_loop_new (NULL, FALSE);
  g_main_loop_run (loop);

  g_bus_unown_name (owner_id);

  g_dbus_node_info_unref (finalnode);
 
  gimp_displays_flush(); // update all the changes we have made to the user interface //

  return;
  //run

  //	g_dbus_interface_info_generate_xml (interface, 0, xml);
  //	fprintf (stderr, "%s\n", xml->str);
  /*
  // Set up a string for debugging
  GString *xml = g_string_new ("");

  //build interfaceinfo
  GDBusInterfaceInfo **interfaces = g_try_malloc (2 * sizeof (GDBusInterfaceInfo *));
  if (interfaces == NULL)
  {
  g_error ("Could not allocate interfaces.");
  } // if (interface == NULL)

  GDBusInterfaceInfo *interface = g_try_malloc (sizeof (GDBusInterfaceInfo));
  fprintf (stderr, "interface is at %p.\n", interface);  
  interface->ref_count = 1;
  interface->name = "edu.grinnell.Glimmer.Test01";
  int i;
  fprintf (stderr, "Does this make a difference.\n");
  printf("so it's here then?");
  //interface->methods = (GDBusMethodInfo **) g_try_malloc (gnames->nprocs * sizeof (GDBusMethodInfo));
  fprintf (stderr, "Does this make a difference2.\n");
  printf("how about here?");
  GDBusMethodInfo **gdbus = (GDBusMethodInfo **) g_try_malloc (gnames->nprocs * sizeof (GDBusMethodInfo));
  for (i = 0; i < gnames->nprocs; i++)
  {
  gdbus[i] = pdb_method_test(gnames->procnames[i]);
  }
  fprintf (stderr, "Does this make a difference3.\n");

  printf("do i get here?");
  //interface->methods[gnames->nprocs+1] = NULL;
 
  interface->signals = 
  (GDBusSignalInfo **) ap (1, 
  g_dbus_signal_info_build ("blah", NULL, NULL));
  interface->properties = 
  (GDBusPropertyInfo **) ap (1, 
  g_dbus_property_info_build ("brah", "i", G_DBUS_PROPERTY_INFO_FLAGS_READABLE, NULL));
  interface->annotations = 
  (GDBusAnnotationInfo **) ap (1,
  g_dbus_annotation_info_build ("edu.grinnell.cs.glimmer.Note", "Hello", NULL));

  interfaces[0] = interface;
  interfaces[1] = NULL;
 

  g_dbus_interface_info_generate_xml (interface, 0, xml);
  fprintf (stderr, "%s\n", xml->str);
  */
#ifdef IGNORE
  printf("isthisignoring?");
  // We're building the methods array by hand.  Eventually, we should try 
  // using to build and populate the array.  (See the call to ap below.)
  interface->methods =  (GDBusMethodInfo **) g_try_malloc (3 * sizeof (GDBusMethodInfo *));
  fprintf (stderr, "methods are at %p\n", interface->methods);
  interface->methods[0] = g_dbus_method_info_build ("foo", NULL, NULL, NULL);
  fprintf (stderr, "foo is at %p.\n", interface->methods[0]);
  interface->methods[1] = g_dbus_method_info_build ("bar", NULL, NULL, NULL);
  fprintf (stderr, "bar is at %p.\n", interface->methods[1]);
  interface->methods[2] = NULL;
#endif
  /*
    interface->methods = (GDBusMethodInfo **) ap (2,
    g_dbus_method_info_build ("foo", NULL, NULL, NULL),
    g_dbus_method_info_build ("bar", NULL, NULL, NULL));
  */

  /*interface->methods = (GDBusMethodInfo **) ap (1,
    pdb_method_test());

    // Right now, there are no signals, properties, or annotations
    interface->signals = NULL;
    interface->properties = NULL;
    interface->annotations = NULL;

    // Set up sample arrays of signals, properties, and such.
    interface->signals = 
    (GDBusSignalInfo **) ap (1, 
    g_dbus_signal_info_build ("blah", NULL, NULL));
    interface->properties = 
    (GDBusPropertyInfo **) ap (1, 
    g_dbus_property_info_build ("brah", "i", G_DBUS_PROPERTY_INFO_FLAGS_READABLE, NULL));
    interface->annotations = 
    (GDBusAnnotationInfo **) ap (1,
    g_dbus_annotation_info_build ("edu.grinnell.cs.glimmer.Note", "Hello", NULL));

    interfaces[0] = interface;
    interfaces[1] = NULL;

    // See what our interface looks like
    g_dbus_interface_info_generate_xml (interface, 0, xml);
    fprintf (stderr, "%s\n", xml->str);

    //builtinterfaceinfo
    */
#ifdef IGNORED					\
  //build node info
  introspection_data = g_try_malloc (sizeof (GDBusNodeInfo));
  introspection_data->ref_count = 1;
  introspection_data->path = NULL;
  introspection_data->interfaces = interfaces;
  gpointer *nodes;
  introspection_data->nodes = (GDBusNodeInfo **) nodes;
  introspection_data->annotations = NULL;
  //built node info
  fprintf (stderr, "Trying to acquire bus.\n");
  owner_id = g_bus_own_name (G_BUS_TYPE_SESSION,
			     "edu.grinnell.cs.glimmer.RAMStest",
			     G_BUS_NAME_OWNER_FLAGS_NONE,
			     on_bus_acquired,
			     on_name_acquired,
			     on_name_lost,
			     NULL,
			     NULL);
  fprintf (stderr, "Sent bus request.\n");

#endif

  


}

