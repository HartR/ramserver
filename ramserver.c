/*
 * ramserver- A program that publishes the GIMP 
 * PDB on the DBUS while handling method calls 
 * to the GIMP from the DBUS.
 *
 * Copyright (c) 2012 Samuel Rebelsky, Hart Russell, and Prashanna Tiwaree
 */


// +---------+
// | Headers |
// +---------+

#include <libgimp/gimp.h>    
#include <libgimp/gimpui.h>    
#include <gio/gio.h>        
#include <stdlib.h>        
#include <stdio.h>      
#include <stdarg.h>
#include <string.h>


// +------------------------------------+
// | Support for Printf-Style Debugging |
// +------------------------------------+

/*
 * When you want messages, use the first version of #define DEBUG.  When you
 * want to turn them off, use the second.
 */

#define DEBUG(...) do { fprintf (stderr, __VA_ARGS__); fprintf (stderr, "\n"); } while (0)

// #define DEBUG(...) do { } while (0)


// +-------+
// | Types |
// +-------+

/*
 * gimpnames struct to help create GDBusMethodInfo
 */
struct gimpnames {
  gchar **procnames;
  gint nprocs;
};


// +---------+
// | Globals |
// +---------+

/*
 * The GDBusNodeInfo to be published to the dbus.
 */
static GDBusNodeInfo *finalnode = NULL;

/**
 * Information on the registration id for this process.
 */
static guint registration_id;


// +-----------------+------------------------------------------------
// | Predeclarations |
// +-----------------+

void rserver_handle_method_call (GDBusConnection       *connection,
                                 const gchar           *method_name,
                                 GVariant              *parameters,
                                 GDBusMethodInvocation *invocation);

// +-----------------+
// | DBUS/GIMP Stuff |
// +-----------------+

//handle_method_call - handles method calls from the dbus
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

  rserver_handle_method_call (connection, 
			      method_name, 
			      parameters, 
			      invocation);
}//handle_method_call


//handle_get_property
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
}//handle_get_property


//handle_set_property
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
}//handle_set_property

//query
static void query (void);

//run
static void run   (const gchar      *name,
                   gint              nparams,
                   const GimpParam  *param,
                   gint             *nreturn_vals,
                   GimpParam       **return_vals);

//interface_vtable
static const GDBusInterfaceVTable interface_vtable =
  {
    handle_method_call,
    handle_get_property,
    handle_set_property
  };//interface_vtable

//on_bus_aquired
static void
on_bus_acquired (GDBusConnection *connection,
                 const gchar     *name,
                 gpointer         user_data)
{
  DEBUG("Bus acquired.");

  registration_id = g_dbus_connection_register_object (connection,
                                                       "/edu/grinnell/cs/glimmer/HARTRULES",
                                                       finalnode->interfaces[0],
                                                       &interface_vtable,
                                                       NULL,  /* user_data */
                                                       NULL,  /* user_data_free_func */
                                                       NULL); /* GERROR */

  DEBUG ("Object registered.");
} // on_bus_acquired

//on_name_acquired
static void
on_name_acquired (GDBusConnection *connection,
                  const gchar     *name,
                  gpointer         user_data)
{
  DEBUG ("Acquired name %s", name);
} // on_name_acquired

//on_name_lost
static void
on_name_lost (GDBusConnection *connection,
              const gchar     *name,
              gpointer         user_data)
{
  DEBUG ("Lost name %s", name);
  exit (1);
} // on_name_lost

//PLUG_IN_INFO
GimpPlugInInfo PLUG_IN_INFO =
  {
    NULL,
    NULL,
    query,
    run
  };//PLUG_IN_INFO

//GIMP MAIN
MAIN()

//query
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
			  "RamServer",
			  "Node Info Test",
			  "Publishes Node on the DBUS",
			  "Hart Russell & Prashanna Tiwaree",
			  "Copyright RAMS 2012",
			  "2012",
			  "<Toolbox>/Xtns/MediaScript/RamServer",
			  NULL, // was "RGB*, GRAY*",
			  GIMP_PLUGIN,
			  G_N_ELEMENTS (args), 0,
			  args, NULL);
} // query


// +----------------------------+
// | Handle Method Call Helpers |
// +----------------------------+

/**
 * Replace one character by another.
 */
gchar *
strrep (gchar *str, gchar target, gchar replacement)
{
  gchar *tmp = str;
  while ((tmp = strchr (tmp, target)) != NULL)
    *tmp = replacement;
  return str;
} // strrep

//Convert a GimpParamDef to a GVariant signature
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
      //SUPPORTING ARRAY TYPES HERE?
    case GIMP_PDB_STRINGARRAY:
      result = (const gchar *) G_VARIANT_TYPE_ARRAY;
      break;
    case GIMP_PDB_INT32ARRAY:
      result = (const gchar *) G_VARIANT_TYPE_ARRAY;
      break;
    case GIMP_PDB_INT16ARRAY:
      result = (const gchar *) G_VARIANT_TYPE_ARRAY;
      break;
    case GIMP_PDB_INT8ARRAY:
      result = (const gchar *) G_VARIANT_TYPE_ARRAY;
      break;
          case GIMP_PDB_FLOATARRAY:
      result = (const gchar *) G_VARIANT_TYPE_ARRAY;
      break;
      //TO HERE
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
} // gimpbus_pdb_param_to_signature


//gimpbus_g_variant_to_gimp_param- converts a single GVariant to a GimpParam, which gets stored 
// in the already allocated param.  Returns success/failure.
gboolean
rserver_g_variant_to_gimp_param (GVariant         *parameter,
                                 GimpPDBArgType    type,
                                 GimpParam        *param)
{

  // Make sure that types match
  const gchar *paramtype = rserver_pdb_type_to_signature (type);
  if (! g_variant_type_equal ((GVariantType *) paramtype,
                              g_variant_get_type (parameter)))
    {
      DEBUG("you're not converting types correctly");
      return FALSE;
    } // g_variant_type_equal

  param->type = type;

  switch (type)
    {
    case GIMP_PDB_INT32:
    case GIMP_PDB_DISPLAY:
    case GIMP_PDB_IMAGE:
    case GIMP_PDB_LAYER:
    case GIMP_PDB_CHANNEL:
    case GIMP_PDB_DRAWABLE:
    case GIMP_PDB_SELECTION:
    case GIMP_PDB_BOUNDARY:
    case GIMP_PDB_VECTORS:
      param->data.d_int32 = g_variant_get_int32 (parameter);
      return TRUE;
 
    case GIMP_PDB_STRING:
      param->data.d_string = g_variant_dup_string (parameter, NULL);
      return TRUE;

    default:
      return FALSE;
    } // switch
} // gimpbus_g_variant_to_gimp_param


// Convert a GVariant to a a newly allocated array of GimpParams.
gboolean
rserver_g_variant_to_gimp_params (GVariant       *parameters, 
                                  GimpParamDef   *types,
                                  GimpParam     **actuals)
{
  int  nparams = g_variant_n_children (parameters);
  GimpParam *result = g_new (GimpParam, nparams);
  int i;

  for (i = 0; i < nparams; i++)
    {
      GVariant *param = g_variant_get_child_value (parameters, i);
      if (! rserver_g_variant_to_gimp_param (param, 
                                             types[i].type, 
                                             &(result[i])))
        {
          g_free (result);
          return FALSE;
        }
    } // for each parameter

  *actuals = result;
  return TRUE;
} // gimpbus_g_variant_to_gimp_params


GVariant *
rserver_gimp_param_to_g_variant (GimpParam value)
{
  switch (value.type)
    {
    case GIMP_PDB_INT32:
    case GIMP_PDB_DISPLAY:
    case GIMP_PDB_IMAGE:
    case GIMP_PDB_LAYER:
    case GIMP_PDB_CHANNEL:
    case GIMP_PDB_DRAWABLE:
    case GIMP_PDB_SELECTION:
    case GIMP_PDB_BOUNDARY:
    case GIMP_PDB_VECTORS:
      return g_variant_new ("i", value.data.d_int32);

    case GIMP_PDB_STRING:
      return g_variant_new ("s", value.data.d_string);

    default:
      return NULL;
    }
} // gimpbus_gimp_param_to_g_variant


GVariant *
rserver_gimp_params_to_g_variant (GimpParam *values, int nvalues)
{
  GVariantBuilder  builder;   
  int              i;         
  GVariant        *val;      
  DEBUG("DO I GET HERE EVEN?");
  g_variant_builder_init (&builder, G_VARIANT_TYPE_TUPLE);
  for (i = 0; i < nvalues; i++)
    {
      DEBUG ("building the list");
      val = rserver_gimp_param_to_g_variant (values[i]);
      DEBUG ("it's going to fail here isn't it?");
            if (val == NULL)
        {
          DEBUG ("failed to add GimpParam ");
          return NULL;
        } // if (val == NULL)

      g_variant_builder_add_value (&builder, val);
      DEBUG ("perhaps here if i'm unlucky?");

    } // for each value

  return g_variant_builder_end (&builder);
} // gimpbus_gimp_params_to_g_variant



void
rserver_handle_method_call (GDBusConnection       *connection,
                            const gchar           *method_name,
                            GVariant              *parameters,
                            GDBusMethodInvocation *invocation)
{

  gchar           *proc_name;
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
  GimpParam       *actuals = NULL;   // The arguments to the call.
  GimpParam       *values = NULL;    // The return values from the call.
  gint             nvalues;          // Number of return values.
  GVariant        *result;

  fprintf (stderr, "Name of the method before %s \n", method_name);

  proc_name = strrep (g_strdup (method_name), '_', '-');

  fprintf (stderr, "Name of the method after %s \n", proc_name);

  DEBUG("WE GET HERE!");


  /*g_dbus_method_invocation_return_dbus_error (invocation,
    "you dun goofed",
    "try again");
  */
  // Look up the information on the procedure in the PDB
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
      DEBUG("invalid procedure call");
      return;
    } // if we can't get the information
  DEBUG("here is the best");

  // Check the number of parameters
  //nparams = g_variant_n_children (parameters);

  // build the parameters
  rserver_g_variant_to_gimp_params (parameters, formals, &actuals);

  DEBUG("TESTING RIGHT MERE");
  // Do the call
  values = gimp_run_procedure2 (proc_name, &nvalues, nparams, actuals);
  DEBUG("over here");


  // Convert the values back to a GVariant
  result = rserver_gimp_params_to_g_variant (values+1, nvalues-1);

  DEBUG("gotta be this one");

  // Return via DBus
  g_dbus_method_invocation_return_value (invocation, result);
  DEBUG("actually, it's this one");

  // Cleanup: TODO
  //g_variant_unref (result);
} // gimpbus_handle_pdb_method_call



// +------------------------+
// | GDBus helper functions |
// +------------------------+


//node_new - creates GDBusNodeInfo to register on DBUS
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

//arg_new - used to help build methodinfo
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
  arg->signature = (gchar *) signature;
  arg->annotations = annotations;
  
  return arg;
} // arg_new

//g_dbus_method_info_build - builds GDBusMethodInfo struct
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



// rserver_pdb_param_to_arginfo - Converts a GimpParamDef to a GDBusArgInfo.
GDBusArgInfo *
rserver_pdb_param_to_arginfo (GimpParamDef param)
{
  gchar *name = strrep (g_strdup (param.name), '-', '_');

  //gchar *name = param.name;
  const gchar *type = rserver_pdb_type_to_signature (param.type);
  GDBusArgInfo *result = arg_new (name, type, NULL);
  if (result == NULL)
    {
      fprintf (stderr, "Could not allocate argument.\n");
    }
  return result;
} // rserver_pdb_param_to_arginfo

//procnamesbuilder - makes a gimpnames struct with all GIMP proc names and # of proc names 
struct gimpnames *
procnamesbuilder ()
{
  struct gimpnames *gimpnamelist;
  gimpnamelist =  g_try_malloc (sizeof (struct gimpnames));
  gimp_procedural_db_query (".*", ".*", ".*", ".*", ".*", ".*", ".*", &gimpnamelist->nprocs, &gimpnamelist->procnames);
  return gimpnamelist;
}//gimpnames


// generate_pdb_method_info - given a PDB proc name, returns the method info
static GDBusMethodInfo *
generate_pdb_method_info (gchar *proc_name)
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
      fprintf (stderr, "ya messed up ya dingus\n");
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
      returns = g_new (GDBusArgInfo *, nreturn_vals+1);
      for (i = 0; i < nreturn_vals ; i++)
        {
         
          returns[i] = rserver_pdb_param_to_arginfo (return_types[i]);
          g_assert (returns[i] != NULL);
        } // for
      // Terminate the array
      returns[nreturn_vals] = NULL;
    } // if (nreturn_vals > 0)

  GDBusMethodInfo * result =
    g_dbus_method_info_build (strrep (g_strdup (proc_name), '-', '_'),//used to be proc_name
			      args,
			      returns,
			      NULL);


  return result;
} // rserver_pdb_method


// methmaker- returns GDBusMethodInfo with all proc info 
GDBusMethodInfo **
methmaker (struct gimpnames *nms)
{
  //DEBUG("does it get here?");
  GDBusMethodInfo **nfo = g_new (GDBusMethodInfo *, nms->nprocs + 1); 
  if (nfo == NULL)
    {
      fprintf (stderr, "Could not allocate method information.\n");
      return NULL;
    }

  int i;

  // BUG
  //   At procedure 309 (at least on Hart's machine), generate_pdb_method_info
  //   fails and causes the program to crash.  It's clearly a memory error.
  //
  //   On Hart's machine, procedure 309 is script-fu-font-map.
  //  
  //   Use of GDB suggests that the error is in gimp_procedural_db_proc_info,
  //   which shouldn't cause a crash.
  //
  //   Valgrind on a different machine did not reveal an error near
  //   script-fu-font-map.  However, it did reveal an invalid write of
  //   size 8 for file-xjt-load.  First, an invalid write of size 8.
  //   Ah hah!  It looks like they did not allocate enough space for
  //   the return values.

  /*
    error: at procedure 309 (maybe 310) generate_pdb_method_info fails and causes the plugin to crash, specifically I think it crashes in 
  */
  fprintf (stderr, "Testing script-fu-font-map as first procedure.\n");
  nfo[0] = generate_pdb_method_info ("script-fu-font-map");
  fprintf (stderr, "Done testing.\n");
  for (i = 0; i < nms->nprocs; i++)
    {
      fprintf (stderr, "%d: %s\n", i, nms->procnames[i]);
      nfo[i] = generate_pdb_method_info (nms->procnames[i]);
    }
  //DEBUG("what about here?");
  // nfo[308]=NULL;
  nfo[nms->nprocs] = NULL;
  //DEBUG("does it get here?");
  return nfo;
}//methmaker

// +-----------+
// | GIMP MAIN |
// +-----------+

static void
run (const gchar      *name,
     gint              nparams,
     const GimpParam  *param,
     gint             *nreturn_vals,
     GimpParam       **return_vals)
{
  static GimpParam  values[1];
  GimpPDBStatusType status = GIMP_PDB_SUCCESS;
  int               pid;    // Process ID; for debugging

  pid = getpid ();

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

  g_dbus_interface_info_generate_xml (interface, 0, xml);
   

  GMainLoop *loop;

  g_type_init ();
 

  DEBUG ("About to make node.");
  finalnode = node_new(NULL, interfaces, NULL, NULL);
  DEBUG ("Made node.");
  /* g_dbus_node_info_generate_xml (finalnode, 0, xml);
     fprintf (stderr, "%s\n", xml->str);
  */


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


  fprintf (stderr, "ramserver process %d is pausing ...\n", pid);
  sleep (6);

  // Event loop.  Wait for functions to get called asyn h.
  loop = g_main_loop_new (NULL, FALSE);
  g_main_loop_run (loop);

  g_bus_unown_name (owner_id);

  g_dbus_node_info_unref (finalnode);
 
  gimp_displays_flush(); // update all the changes we have made to the user interface //

  return;

}



