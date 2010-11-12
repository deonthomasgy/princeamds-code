#include <E_DBus.h>
#include <Ecore_Getopt.h>
#define INT 0
#define  DOUBLE 2
#define STRING 1


typedef struct _remote_client_vars Remote_Client_Vars;

typedef enum _Input_Type
{
	INPUT_TYPE_NONE,
	INPUT_TYPE_INT,
	INPUT_TYPE_DOUBLE,
	INPUT_TYPE_STRING,
	INPUT_TYPE_FLOAT
	
} Input_Type;

struct _remote_client_vars
{
	char *val[6];
	const char *argv;
	const char *dbus_obj;
	const char *dbus_name;
	int vals[3]; // 0 = int, 1 = string , 2 = Double
	Input_Type input_type;
	Eina_Bool quit_bool;
	//Eina_Bool found;
	Eina_Bool null;
};
Remote_Client_Vars *config;
static int input_type = INPUT_TYPE_NONE;

// Callback, Dbus fucntion, Dbus Action, Int, Str , Double
char *values[ ] [6] =
{
	{ "--default-bg-get","org.enlightenment.Remote.Wallpaper", "Default_Get" , "0" , "0" "0"},
	{ "--default-bg-config-get","org.enlightenment.Remote.Wallpaper", "Config_Get" , "0" , "0" "0"},
	{ "--default-bg-set","org.enlightenment.Remote.Wallpaper", "Default_Set" , "0" , "1" , "0" },
	{ "--default-bg-add","org.enlightenment.Remote.Wallpaper", "Add" , "4" , "1" , "0" },
	{ "--default-bg-del","org.enlightenment.Remote.Wallpaper", "Del" , "4" , "0" , "0" },
	{ "--bg-set-this-screen","org.enlightenment.Remote.Wallpaper", "Set_This_Screen" , "0" , "1" , "0" },
	{ "--bg-set-current-desktop","org.enlightenment.Remote.Wallpaper", "Set_Current_Desktop" , "0" , "1" , "0" },
	{ "--bg-get-specific-desktop","org.enlightenment.Remote.Wallpaper", "Get" , "4" , "0" , "0" },
	{ "--lang-get","org.enlightenment.Remote.Intl", "Get" , "0" , "0" , "0" },
	{ "--lang-set","org.enlightenment.Remote.Intl", "Set" , "0" , "1" , "0" },
	{ "--lang-list","org.enlightenment.Remote.Intl", "List" , "0" , "0" , "0" },
	{ "--hibernate","org.enlightenment.Remote.Core", "Hibernate" , "0" , "0" , "0" },
	{ "--suspend","org.enlightenment.Remote.Core", "Suspend" , "0" , "0" , "0" },
	{ "--logout","org.enlightenment.Remote.Core", "Logout" , "0" , "0" , "0" },
	{ "--reboot","org.enlightenment.Remote.Core", "Reboot" , "0" , "0" , "0" },
	{ "--halt","org.enlightenment.Remote.Core", "Halt" , "0" , "0" , "0" },
	{ "--halt-now","org.enlightenment.Remote.Core", "Halt_Now" , "0" , "0" , "0" },
	{ "--lock","org.enlightenment.Remote.Core", "Lock" , "0" , "0" , "0" },
	{ "--execute","org.enlightenment.Remote.Core", "Execute" , "0" , "1" , "0" },
	{ "--save-config","org.enlightenment.Remote.Core", "Save_Config" , "0" , "0" , "0" },
	{ "--theme-set","org.enlightenment.Remote.Theme", "Set" , "0" , "2" , "0" },
	{ "--theme-get","org.enlightenment.Remote.Theme", "Get" , "0" , "1" , "0" },
	{ "--theme-list","org.enlightenment.Remote.Theme", "List" , "0" , "0" , "0" },
	{ "--theme-add","org.enlightenment.Remote.Theme", "Add" , "0" , "1" , "0" },
	{ "--theme-del","org.enlightenment.Remote.Theme", "Del" , "0" , "1" , "0" },
	{ "--theme-list-user-themes","org.enlightenment.Remote.Theme", "List_User_Themes" , "0" , "0" , "0" },
	{ "--framerate-get","org.enlightenment.Remote.Framerate", "Get" , "0" , "0" , "0" },
	{ "--framerate-set","org.enlightenment.Remote.Framerate", "Set" , "0" , "0" , "1" },
	{ "--scrollspeed-get","org.enlightenment.Remote.Scrollspeed", "Get" , "0" , "0" , "0" },
	{ "--scrollspeed-set","org.enlightenment.Remote.Scrollspeed", "Set" , "0" , "0" , "1" },
	{ "--focus-get","org.enlightenment.Remote.Focus", "Get" , "0" , "0" , "0" },
	{ "--focus-set","org.enlightenment.Remote.Focus", "Set" , "0" , "1" , "0" },
	{ "--cache-fonts-get","org.enlightenment.Remote.Fonts", "Cache_Get" , "0" , "0" , "0" },
	{ "--cache-fonts-set","org.enlightenment.Remote.Fonts", "Cache_Set" , "0" , "0" , "1" },
	{ "--cache-image-get","org.enlightenment.Remote.Image", "Cache_Get" , "0" , "0" , "0" },
	{ "--cache-image-set","org.enlightenment.Remote.Image", "Cache_Set" , "0" , "0" , "1" },
	{ "--cache-edje-get","org.enlightenment.Remote.Edje", "Cache_Get" , "0" , "0" , "0" },
	{ "--cache-edje-set","org.enlightenment.Remote.Edje", "Cache_Set" , "1" , "0" , "0" },
	{ "--collection-cache-edje-get","org.enlightenment.Remote.Edje", "Collection_Cache_Get" , "0" , "0" , "0" },
	{ "--collection-cache-edje-set","org.enlightenment.Remote.Edje", "Collection_Cache_Set" , "1" , "0" , "0" },
	{ "--resolution-get","org.enlightenment.Remote.Resolution", "Get" , "0" , "0" , "0" },
	{ "--resolution-set","org.enlightenment.Remote.Resolution", "Set" , "1" , "0" , "0" },
	{ "--resolution-list","org.enlightenment.Remote.Resolution", "List" , "0" , "0" , "0" },
	{ "--test1","NULL", "NULL" , "2" , "0" , "0" },
	{ "--test2","NULL", "NULL" , "0" , "2" , "0" },
	{ "--test3","NULL", "NULL" , "0" , "0" , "2" },
	{ NULL , NULL , NULL  , NULL , NULL , NULL }
};

/*dbus functions*/

static void
print_iter (DBusMessageIter *iter, int depth)
{
	do
	{
		int type = dbus_message_iter_get_arg_type (iter);
	
		if (type == DBUS_TYPE_INVALID)
			break;
		
		switch (type)
		{
			case DBUS_TYPE_STRING:
			{
				char *val;
				dbus_message_iter_get_basic (iter, &val);
				printf ("string \"");
				printf ("%s", val);
				printf ("\"\n");
				break;
			}
			
			case DBUS_TYPE_INT32:
			{
				dbus_int32_t val;
				dbus_message_iter_get_basic (iter, &val);
				printf ("int32 %d\n", val);
				break;
			}
			
			case DBUS_TYPE_DOUBLE:
			{
				double val;
				dbus_message_iter_get_basic (iter, &val);
				printf ("double %g\n", val);
				break;
			}
			
			case DBUS_TYPE_ARRAY:
			{
				int current_type;
				DBusMessageIter subiter;
	
				dbus_message_iter_recurse (iter, &subiter);

				printf("array [\n");
				while ((current_type = dbus_message_iter_get_arg_type (&subiter)) != DBUS_TYPE_INVALID)
				{
					print_iter (&subiter,depth+1);
					dbus_message_iter_next (&subiter);
					if (dbus_message_iter_get_arg_type (&subiter) != DBUS_TYPE_INVALID)
						printf (",");
				}
				printf("]\n");
				break;
			}
		}
	}
	while (dbus_message_iter_next (iter));
}
void
cb_reply(void *data, DBusMessage *reply, DBusError *error)
{
	const char *val;
	char *array;
	
	int current_type;
	DBusMessageIter subiter;
	DBusMessageIter iter;
	dbus_message_iter_init (reply, &iter);
	
	print_iter(&iter,1);

	ecore_main_loop_quit();
}

int
_message(E_DBus_Connection *conn)
{

	DBusMessage *msg;
	DBusMessageIter iter;
	
	msg = dbus_message_new_method_call(
		"org.enlightenment.Remote.service",
		"/org/enlightenment/Remote/RemoteObject",
		config->dbus_obj,config->dbus_name
	);
  
	/*
	printf("Val[0]:%s\n",config->val[0]);
	printf("Val[1]:%s\n",config->val[1]);
	printf("Val[2]:%s\n",config->val[2]);
	printf("Val[3]:%s\n",config->val[3]);
	printf("Val[4]:%s\n",config->val[4]);
	printf("Val[5]:%s\n",config->val[5]);
*/
	
	dbus_message_iter_init_append(msg, &iter);
	
	int v,w,i=0;
	int ival;
	double dval;
	
	//How many Intergers , Strings , Doubles
	switch (input_type)
	  {
		  case INPUT_TYPE_NONE:
			  break;
		  case 
			  INPUT_TYPE_INT:
			printf("TYPE:INT\n");
			for (i = 0 ; i < config->vals[INT]; i++)
			{
				printf("MESSAGE:%s\n",config->val[i]);
				ival = atoi(config->val[i]);
				dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &ival);
			}
			break;
		  case INPUT_TYPE_DOUBLE:
			printf("TYPE:DOUBLE\n");
			for (i = 0 ; i < config->vals[DOUBLE]; i++)
			{
				printf("MESSAGE:%s\n",config->val[i]);
				dval = atof(config->val[i]);
				dbus_message_iter_append_basic(&iter, DBUS_TYPE_DOUBLE, &dval);
			}
			  break;
		  case INPUT_TYPE_STRING:
			  printf("TYPE:STRING %d\n",i);
			for (i = 0 ; i < config->vals[STRING]; i++)
			{
				printf("MESSAGE:%s\n",config->val[i]);
				dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &config->val[i]);
			}
			  break;
	  }
	
	printf("i'm at the end\n");
	e_dbus_message_send(conn, msg, cb_reply, 10, NULL);
	dbus_message_unref(msg);
	printf("SUCCESS\n");
  return 0;
}

/*dbus functions end*/

const Ecore_Getopt general_opts = 
{
	"enlightenment_remote_module_client",
	NULL,
	"0.2",
	"(C) 2009-2011 - PrinceAMD <princeamd.elive@gmail.com>",
	"GPL v3 - GNU General Public License",
	"Remote module Client\n"
	"\n"
	"These are options which you can use \n "
	"ERMC Version 0.2",
	0,
	{
		ECORE_GETOPT_COUNT
		('\0', "default-bg-get", "Get the default background edje file path."),
		ECORE_GETOPT_COUNT
		('\0', "default-bg-config-get", "Get the default background config."),
		ECORE_GETOPT_STORE_STR
		('\0', "default-bg-set", "  OPT1 Set the default background edje to the desktop \n"
			"background in the file 'OPT1' (must be a full path)"),
		ECORE_GETOPT_STORE_STR
		('\0', "default-bg-add", "OPT1 OPT2 OPT3 OPT4 OPT5 Add a desktop bg definition. \n" 
			"OPT1 = container no. OPT2 = zone no. OPT3 = desk_x. OPT4 = desk_y. OPT5 = bg file path"),
		ECORE_GETOPT_STORE_STR
		('\0', "default-bg-del", "OPT1 OPT2 OPT3 OPT4 Delete a desktop bg definition.\n " 
			"OPT1 = container no. OPT2 = zone no. OPT3 = desk_x. OPT4 = desk_y."),
		ECORE_GETOPT_STORE_STR ('\0', "bg-set-this-screen", "Set a specific wallpaper on this screen"),
		ECORE_GETOPT_STORE_STR ('\0', "bg-set-current-desktop", "Set a specific wllpaper on this desktop alone"),
		ECORE_GETOPT_STORE_STR ('\0', "bg-get-specific-desktop", "Get the name of the wallpaper on a specific screen"),
		ECORE_GETOPT_COUNT  ('\0', "lang-get", "INFO"),
		ECORE_GETOPT_STORE_STR  ('\0', "lang-set", "INFO"),
		ECORE_GETOPT_COUNT  ('\0', "lang-list", "INFO"),
		ECORE_GETOPT_COUNT  ('\0', "hibernate", "INFO"),
		ECORE_GETOPT_COUNT  ('\0', "suspend", "INFO"),
		ECORE_GETOPT_COUNT  ('\0', "logout", "INFO"),
		ECORE_GETOPT_COUNT  ('\0', "reboot", "INFO"),
		ECORE_GETOPT_COUNT  ('\0', "halt", "INFO"),
		ECORE_GETOPT_COUNT  ('\0', "halt-now", "INFO"),
		ECORE_GETOPT_COUNT  ('\0', "lock", "INFO"),
		ECORE_GETOPT_STORE_STR  ('\0', "execute", "INFO"),
		ECORE_GETOPT_COUNT  ('\0', "save-config", "INFO"),
		ECORE_GETOPT_STORE_STR  ('\0', "theme-set", "INFO"),
		ECORE_GETOPT_STORE_STR  ('\0', "theme-get", "INFO"),
		ECORE_GETOPT_COUNT  ('\0', "theme-list", "INFO"),
		ECORE_GETOPT_STORE_STR  ('\0', "theme-add", "INFO"),
		ECORE_GETOPT_STORE_STR  ('\0', "theme-del", "INFO"),
		ECORE_GETOPT_COUNT  ('\0', "theme-list-user-themes", "INFO"),
		ECORE_GETOPT_COUNT  ('\0', "framerate-get", "INFO"),
		ECORE_GETOPT_STORE_STR  ('\0', "framerate-set", "INFO"),
		ECORE_GETOPT_COUNT  ('\0', "scrollspeed-get", "INFO"),
		ECORE_GETOPT_STORE_STR  ('\0', "scrollspeed-set", "INFO"),
		ECORE_GETOPT_COUNT  ('\0', "focus-get", "INFO"),
		ECORE_GETOPT_STORE_STR  ('\0', "focus-set", "INFO"),
		ECORE_GETOPT_COUNT  ('\0', "cache-fonts-get", "INFO"),
		ECORE_GETOPT_STORE_STR  ('\0', "cache-fonts-set", "INFO"),
		ECORE_GETOPT_COUNT  ('\0', "cache-image-get", "INFO"),
		ECORE_GETOPT_STORE_STR  ('\0', "cache-image-set", "INFO"),
		ECORE_GETOPT_COUNT  ('\0', "cache-edje-get", "INFO"),
		ECORE_GETOPT_STORE_STR  ('\0', "cache-edje-set", "INFO"),
		ECORE_GETOPT_COUNT  ('\0', "collection-cache-edje-get", "INFO"),
		ECORE_GETOPT_STORE_STR  ('\0', "collection-cache-edje-set", "INFO"),
		ECORE_GETOPT_COUNT  ('\0', "resolution-get", "INFO"),
		ECORE_GETOPT_STORE_STR  ('\0', "resolution-set", "INFO"),
		ECORE_GETOPT_COUNT  ('\0', "resolution-list", "INFO"),
		ECORE_GETOPT_STORE_STR  ('\0', "test1", "INFO"),
		ECORE_GETOPT_STORE_STR  ('\0', "test2", "INFO"),
		ECORE_GETOPT_STORE_STR  ('\0', "test3", "INFO"),
		ECORE_GETOPT_LICENSE('L', "license"),
		ECORE_GETOPT_COPYRIGHT('C', "copyright"),
		ECORE_GETOPT_VERSION('V', "version"),
		ECORE_GETOPT_HELP('h', "help"),
		ECORE_GETOPT_SENTINEL
	}
};

static Eina_Bool
_args_init(int argc, char **argv)
{
	config = calloc(1, sizeof(Remote_Client_Vars));
	int arg_index, i,run;
	
	Ecore_Getopt_Value general_values[] = {
		ECORE_GETOPT_VALUE_BOOL(config->null),
		ECORE_GETOPT_VALUE_BOOL(config->null),
		ECORE_GETOPT_VALUE_STR(config->val[0]),
		ECORE_GETOPT_VALUE_STR(config->val[0]),
		ECORE_GETOPT_VALUE_STR(config->val[0]),
		ECORE_GETOPT_VALUE_STR(config->val[0]),
		ECORE_GETOPT_VALUE_STR(config->val[0]),
		ECORE_GETOPT_VALUE_STR(config->val[0]),
		ECORE_GETOPT_VALUE_BOOL(config->null),
		ECORE_GETOPT_VALUE_STR(config->val[0]),
		ECORE_GETOPT_VALUE_BOOL(config->null),
		ECORE_GETOPT_VALUE_BOOL(config->null),
		ECORE_GETOPT_VALUE_BOOL(config->null),
		ECORE_GETOPT_VALUE_BOOL(config->null),
		ECORE_GETOPT_VALUE_BOOL(config->null),
		ECORE_GETOPT_VALUE_BOOL(config->null),
		ECORE_GETOPT_VALUE_BOOL(config->null),
		ECORE_GETOPT_VALUE_BOOL(config->null),
		ECORE_GETOPT_VALUE_STR(config->val[0]),
		ECORE_GETOPT_VALUE_BOOL(config->null),
		ECORE_GETOPT_VALUE_STR(config->val[0]),
		ECORE_GETOPT_VALUE_STR(config->val[0]),
		ECORE_GETOPT_VALUE_BOOL(config->null),
		ECORE_GETOPT_VALUE_STR(config->val[0]),
		ECORE_GETOPT_VALUE_STR(config->val[0]),
		ECORE_GETOPT_VALUE_BOOL(config->null),
		ECORE_GETOPT_VALUE_BOOL(config->null),
		ECORE_GETOPT_VALUE_STR(config->val[0]),
		ECORE_GETOPT_VALUE_BOOL(config->null),
		ECORE_GETOPT_VALUE_STR(config->val[0]),
		ECORE_GETOPT_VALUE_BOOL(config->null),
		ECORE_GETOPT_VALUE_STR(config->val[0]),
		ECORE_GETOPT_VALUE_BOOL(config->null),
		ECORE_GETOPT_VALUE_STR(config->val[0]),
		ECORE_GETOPT_VALUE_BOOL(config->null),
		ECORE_GETOPT_VALUE_STR(config->val[0]),
		ECORE_GETOPT_VALUE_BOOL(config->null),
		ECORE_GETOPT_VALUE_STR(config->val[0]),
		ECORE_GETOPT_VALUE_BOOL(config->null),
		ECORE_GETOPT_VALUE_STR(config->val[0]),
		ECORE_GETOPT_VALUE_BOOL(config->null),
		ECORE_GETOPT_VALUE_STR(config->val[0]),
		ECORE_GETOPT_VALUE_STR(config->val[0]),
		ECORE_GETOPT_VALUE_STR(config->val[0]),
		ECORE_GETOPT_VALUE_STR(config->val[0]),
		ECORE_GETOPT_VALUE_BOOL(config->null),
		ECORE_GETOPT_VALUE_BOOL(config->quit_bool),
		ECORE_GETOPT_VALUE_BOOL(config->quit_bool),
		ECORE_GETOPT_VALUE_BOOL(config->quit_bool),
		ECORE_GETOPT_VALUE_BOOL(config->quit_bool),
		ECORE_GETOPT_VALUE_NONE
	};
	
	if(!config->null)
		arg_index = ecore_getopt_parse(&general_opts, general_values, argc, argv);
	else if(config->quit_bool)
		arg_index = ecore_getopt_parse(&general_opts, general_values, argc, argv);
	
	run = 1;
	i = 0;
	
	while(run)
	{
		if(!values[i][0])
		{
			break;
		}
		if((argc > 1) && (!config->quit_bool))
		{
			if(!strcmp(argv[1], values[i][0]))
			{
				int v,w,x=0;
				
				config->dbus_obj = eina_stringshare_add(values[i][1]);
				config->dbus_name = eina_stringshare_add(values[i][2]);
				
				//How many Intergers , Strings , Doubles
				input_type = INPUT_TYPE_NONE;
				for (w = 3; w <= 5; w++)
				{
					if((config->vals[x] = atoi(values[i][w])) >=1)
					{
						//printf("val[%d]=%d\n",x,config->vals[x]);
						if(x == INT )
							input_type = INPUT_TYPE_INT;
						else if ( x == STRING )
							input_type = INPUT_TYPE_STRING;
						else if ( x == DOUBLE )
							input_type = INPUT_TYPE_DOUBLE;
						
						for (v = 0; v < config->vals[x]; v++)
						{
							config->val[v] = argv[2+v];
						}
					}x++;
				}
			}
		}i++;
	}
	
	if(config->quit_bool)
	{
		ecore_main_loop_quit();
	}
	
return EINA_TRUE;
}

int main(int argc, char **argv)
{
	ecore_init();
	e_dbus_init();
	E_DBus_Connection *conn;
	
	conn = e_dbus_bus_get(DBUS_BUS_SESSION);
	
	_args_init(argc, argv);
	
	if((argc > 1) && (!config->quit_bool))
		_message(conn);
	else
	ecore_main_loop_quit();
	
	ecore_main_loop_begin();
	ecore_shutdown();
}
