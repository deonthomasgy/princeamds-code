#include <e.h>
#include <E_DBus.h>
#include "e_mod_main.h"

typedef struct _Remote_Data Remote_Data;

struct _Remote_Data 
{
   E_DBus_Connection *conn;
   E_DBus_Object     *obj;
};

static Remote_Data *data = NULL;

EAPI E_Module_Api e_modapi = {E_MODULE_API_VERSION, "remote"};

/*
 * Module Functions
 */
EAPI void *
e_modapi_init(E_Module *m) 
{
	e_dbus_init();
	e_remote_init();
   return m;
}

/*
 * Function to unload the module
 */
EAPI int 
e_modapi_shutdown(E_Module *m) 
{
   if (!(restart) && !(stopping))
      e_util_dialog_show( "Warning", "This module is NEEDED for Elive for misc things, you should NOT disable it or your system may work incorrectly");
   
    if (data->obj)
     {
        e_dbus_object_free(data->obj);
     }
   if (data->conn)
     {
        e_dbus_connection_close(data->conn);
     }
   e_dbus_shutdown();
     
   E_FREE(data);
   data = NULL;
    return 1;
}

/*
 * Function to Save the modules config
 */ 
EAPI int 
e_modapi_save(E_Module *m)
{
   return 1;
}


/*Intl Handlers*/
static DBusMessage* 
_e_remote_get_intl_cb(E_DBus_Object *obj, DBusMessage *msg)
{
    DBusMessageIter iter;
   DBusMessage *reply;
   const char *intl;

   intl = e_intl_language_get();

   reply = dbus_message_new_method_return(msg);
   dbus_message_iter_init_append(reply, &iter);
   dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &intl);

   return reply;
}
static DBusMessage* 
_e_remote_set_intl_cb(E_DBus_Object *obj, DBusMessage *msg)
{
   DBusMessageIter iter;
   const char *intl; 

   dbus_message_iter_init(msg, &iter);
   dbus_message_iter_get_basic(&iter, &intl);

   if (e_config->language) eina_stringshare_del(e_config->language);
   e_config->language = NULL;
   if (intl) e_config->language = eina_stringshare_add(intl);
   if ((e_config->language) && (e_config->language[0] != 0))
   e_intl_language_set(intl);
   e_config_save_queue();
   e_sys_action_do(E_SYS_RESTART, NULL);
   return dbus_message_new_method_return(msg);
}

static DBusMessage* 
_e_remote_list_intl_cb(E_DBus_Object *obj, DBusMessage *msg)
{
   Eina_List *l;
   const char *name;
   DBusMessage *reply;
   DBusMessageIter iter;
   DBusMessageIter arr;

   reply = dbus_message_new_method_return(msg);
   dbus_message_iter_init_append(reply, &iter);
   dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY, "s", &arr);

   EINA_LIST_FOREACH(e_intl_language_list(), l, name)
     {
	dbus_message_iter_append_basic(&arr, DBUS_TYPE_STRING, &name);
     }
   dbus_message_iter_close_container(&iter, &arr);

   return reply;
}

/*Wallpaper Handlers*/
static DBusMessage* 
_e_remote_set_wall_cb(E_DBus_Object *obj, DBusMessage *msg)
{
	DBusMessageIter iter;
	char *bg; 

	dbus_message_iter_init(msg, &iter);
	dbus_message_iter_get_basic(&iter, &bg);

	while (e_config->desktop_backgrounds)
          {
             E_Config_Desktop_Background *cfbg;
             
             cfbg = e_config->desktop_backgrounds->data;
             e_bg_del(cfbg->container, cfbg->zone, cfbg->desk_x, cfbg->desk_y);
          }
	  
	e_bg_default_set(bg);
	e_bg_update();
	e_config_save_queue();
   return dbus_message_new_method_return(msg);
}

static DBusMessage* 
_e_remote_set_current_desktop_wall_cb(E_DBus_Object *obj, DBusMessage *msg)
{
	DBusMessageIter iter;
	char *bg; 
	int container, _zone, desk_x, desk_y;
	E_Zone *zone;
	E_Desk *desk;
	E_Container *con;

	con = e_container_current_get(e_manager_current_get());
	zone = e_util_zone_current_get(con->manager);
	desk = e_desk_current_get(zone);
	
	container = con->num;
	_zone = zone->id;
	desk_x = desk->x;
	desk_y = desk->y;
	
	dbus_message_iter_init(msg, &iter);
	dbus_message_iter_get_basic(&iter, &bg);

	e_bg_del(container,_zone,desk_x,desk_y);
	e_bg_add(container,_zone,desk_x,desk_y,bg);
	e_config_save_queue();
	e_bg_update();
   return dbus_message_new_method_return(msg);
}

static DBusMessage* 
_e_remote_get_wall_cb(E_DBus_Object *obj, DBusMessage *msg)
{
   DBusMessageIter iter;
   DBusMessage *reply;
   int container, zone, desk_x, desk_y;
   const char *bg;

   dbus_message_iter_init(msg, &iter);
   dbus_message_iter_get_basic(&iter, &container);
   dbus_message_iter_next(&iter);
   dbus_message_iter_get_basic(&iter, &zone);
   dbus_message_iter_next(&iter);
   dbus_message_iter_get_basic(&iter, &desk_x);
   dbus_message_iter_next(&iter);
   dbus_message_iter_get_basic(&iter, &desk_y);
   bg = e_bg_file_get(container,zone,desk_x,desk_y);
   reply = dbus_message_new_method_return(msg);
   dbus_message_iter_init_append(reply, &iter);
   dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &bg);

   return reply;
}

static DBusMessage* 
_e_remote_default_get_wall_cb(E_DBus_Object *obj, DBusMessage *msg)
{
   DBusMessageIter iter;
   DBusMessage *reply;
   int container, zone, desk_x, desk_y;
   const char *bg;
 
   bg = e_config->desktop_default_background;
   reply = dbus_message_new_method_return(msg);
   dbus_message_iter_init_append(reply, &iter);
   dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &bg);

   return reply;
}

static DBusMessage* 
_e_remote_del_wall_cb(E_DBus_Object *obj, DBusMessage *msg)
{
   DBusMessageIter iter;
   int container, zone, desk_x, desk_y;

   dbus_message_iter_init(msg, &iter);
   dbus_message_iter_get_basic(&iter, &container);
   dbus_message_iter_next(&iter);
   dbus_message_iter_get_basic(&iter, &zone);
   dbus_message_iter_next(&iter);
   dbus_message_iter_get_basic(&iter, &desk_x);
   dbus_message_iter_next(&iter);
   dbus_message_iter_get_basic(&iter, &desk_y);
   e_bg_del(container,zone,desk_x,desk_y);
   e_config_save_queue();
   e_bg_update();

  return dbus_message_new_method_return(msg);
}

static DBusMessage* 
_e_remote_add_wall_cb(E_DBus_Object *obj, DBusMessage *msg)
{
   DBusMessageIter iter;
   DBusMessage *reply;
   int container, zone, desk_x, desk_y;
   char *bg;

   reply = dbus_message_new_method_return(msg);
   dbus_message_iter_init(msg, &iter);
   dbus_message_iter_get_basic(&iter, &container);
   dbus_message_iter_next(&iter);
   dbus_message_iter_get_basic(&iter, &zone);
   dbus_message_iter_next(&iter);
   dbus_message_iter_get_basic(&iter, &desk_x);
   dbus_message_iter_next(&iter);
   dbus_message_iter_get_basic(&iter, &desk_y);
   dbus_message_iter_next(&iter);
   dbus_message_iter_get_basic(&iter, &bg);
   e_bg_add(container,zone,desk_x,desk_y,bg);
   e_config_save_queue();
   e_bg_update();

  return dbus_message_new_method_return(msg);
}

static DBusMessage* 
_e_remote_core_hibernate_cb(E_DBus_Object *obj, DBusMessage *msg)
{
   e_sys_action_do(E_SYS_HIBERNATE, NULL);
   return dbus_message_new_method_return(msg);
}

static DBusMessage* 
_e_remote_core_suspend_cb(E_DBus_Object *obj, DBusMessage *msg)
{
   e_sys_action_do(E_SYS_SUSPEND, NULL);
   return dbus_message_new_method_return(msg);
}

static DBusMessage* 
_e_remote_core_logout_cb(E_DBus_Object *obj, DBusMessage *msg)
{
   e_sys_action_do(E_SYS_LOGOUT, NULL);
   return dbus_message_new_method_return(msg);
}

static DBusMessage* 
_e_remote_core_reboot_cb(E_DBus_Object *obj, DBusMessage *msg)
{
   e_sys_action_do(E_SYS_REBOOT, NULL);
   return dbus_message_new_method_return(msg);
}

static DBusMessage* 
_e_remote_core_halt_cb(E_DBus_Object *obj, DBusMessage *msg)
{
   e_sys_action_do(E_SYS_HALT, NULL);
   return dbus_message_new_method_return(msg);
}

static DBusMessage* 
_e_remote_core_halt_now_cb(E_DBus_Object *obj, DBusMessage *msg)
{
   e_sys_action_do(E_SYS_HALT_NOW, NULL);
   return dbus_message_new_method_return(msg);
}

static DBusMessage* 
_e_remote_core_lock_cb(E_DBus_Object *obj, DBusMessage *msg)
{
   e_desklock_show();
   return dbus_message_new_method_return(msg);
}

static DBusMessage* 
_e_remote_core_save_config_cb(E_DBus_Object *obj, DBusMessage *msg)
{
   e_config_save();
   return dbus_message_new_method_return(msg);
}

static DBusMessage* 
_e_remote_set_theme_cb(E_DBus_Object *obj, DBusMessage *msg)
{
   DBusMessageIter iter;
   char *category; 
   char *edje_file; 
   
   dbus_message_iter_init(msg, &iter);
   dbus_message_iter_get_basic(&iter, &category);
   dbus_message_iter_next(&iter);
   dbus_message_iter_get_basic(&iter, &edje_file);

   e_theme_config_set(category,edje_file);
   e_config_save_queue();
   e_sys_action_do(E_SYS_RESTART, NULL);
   return dbus_message_new_method_return(msg);
}

static DBusMessage* 
_e_remote_get_theme_cb(E_DBus_Object *obj, DBusMessage *msg)
{
   DBusMessageIter iter;
   DBusMessage *reply;
   E_Config_Theme *ect;
   char *category;
   
   dbus_message_iter_init(msg, &iter);
   dbus_message_iter_get_basic(&iter, &category);
	
   ect = e_theme_config_get(category);

   reply = dbus_message_new_method_return(msg);
   dbus_message_iter_init_append(reply, &iter);
   dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &ect->file);

   return reply;
}

static DBusMessage* 
_e_remote_list_theme_cb(E_DBus_Object *obj, DBusMessage *msg)
{
   Eina_List *l;
   E_Config_Theme *ect = NULL;
   DBusMessage *reply;
   DBusMessageIter iter;
   DBusMessageIter arr;

   reply = dbus_message_new_method_return(msg);
   dbus_message_iter_init_append(reply, &iter);
   dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY, "s", &arr);
   ect = e_theme_config_get("theme");
	
   EINA_LIST_FOREACH(e_config->themes, l, ect)
     {
	dbus_message_iter_append_basic(&arr, DBUS_TYPE_STRING, &ect->file);
     }
   dbus_message_iter_close_container(&iter, &arr);

   return reply;
}

static DBusMessage* 
_e_remote_get_framerate_cb(E_DBus_Object *obj, DBusMessage *msg)
{
   DBusMessageIter iter;
   DBusMessage *reply;
   double frame;

   frame = e_config->framerate;

   reply = dbus_message_new_method_return(msg);
   dbus_message_iter_init_append(reply, &iter);
   dbus_message_iter_append_basic(&iter, DBUS_TYPE_DOUBLE, &frame);

   return reply;
}

static DBusMessage* 
_e_remote_set_framerate_cb(E_DBus_Object *obj, DBusMessage *msg)
{
   DBusMessageIter iter;
   double frame; 

   dbus_message_iter_init(msg, &iter);
   dbus_message_iter_get_basic(&iter, &frame);

   e_config->framerate = frame;
   E_CONFIG_LIMIT(e_config->framerate, 1.0, 240.0);
   edje_frametime_set(1.0 / e_config->framerate);
   e_config_save_queue();
   return dbus_message_new_method_return(msg);
}

static DBusMessage* 
_e_remote_get_scrollspeed_cb(E_DBus_Object *obj, DBusMessage *msg)
{
   DBusMessageIter iter;
   DBusMessage *reply;
   double scrollspeed;

   scrollspeed = e_config->menus_scroll_speed;

   reply = dbus_message_new_method_return(msg);
   dbus_message_iter_init_append(reply, &iter);
   dbus_message_iter_append_basic(&iter, DBUS_TYPE_DOUBLE, &scrollspeed);

   return reply;
}

static DBusMessage* 
_e_remote_set_scrollspeed_cb(E_DBus_Object *obj, DBusMessage *msg)
{
   DBusMessageIter iter;
   double scrollspeed; 

   dbus_message_iter_init(msg, &iter);
   dbus_message_iter_get_basic(&iter, &scrollspeed);

   e_config->menus_scroll_speed = scrollspeed;
   E_CONFIG_LIMIT(e_config->menus_scroll_speed, 1.0, 20000.0);
   e_config_save_queue();
   return dbus_message_new_method_return(msg);
}

static DBusMessage* 
_e_remote_get_focus_cb(E_DBus_Object *obj, DBusMessage *msg)
{
   DBusMessageIter iter;
   DBusMessage *reply;
   char *focus;
   int value = 0;

   value = e_config->focus_policy;
   if(value == E_FOCUS_CLICK)
       focus = "CLICK";
   else if (value == E_FOCUS_MOUSE)
	focus = "MOUSE";
   else if (value == E_FOCUS_SLOPPY)
	focus = "SLOPPY";
   
   reply = dbus_message_new_method_return(msg);
   dbus_message_iter_init_append(reply, &iter);
   dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &focus);

   return reply;
}

static DBusMessage* 
_e_remote_set_focus_cb(E_DBus_Object *obj, DBusMessage *msg)
{
   DBusMessageIter iter;
   DBusMessage *reply;
   char *focus; 
   char *error;
   int value = 0;

   dbus_message_iter_init(msg, &iter);
   dbus_message_iter_get_basic(&iter, &focus);

    if (!strcmp(focus, "MOUSE")) value = E_FOCUS_MOUSE;
   else if (!strcmp(focus, "CLICK")) value = E_FOCUS_CLICK;
   else if (!strcmp(focus, "SLOPPY")) value = E_FOCUS_SLOPPY;
   else
     {
	 error = "focus must be MOUSE, CLICK or SLOPPY\n";
	 reply = dbus_message_new_method_return(msg);
	 dbus_message_iter_init_append(reply, &iter);
	 dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &error);
   return reply;
     }
   e_border_button_bindings_ungrab_all();
   e_config->focus_policy = value;
   E_CONFIG_LIMIT(e_config->focus_policy, 0, 2);
   e_border_button_bindings_grab_all();
   e_config_save_queue();
   return dbus_message_new_method_return(msg);
}

static DBusMessage* 
_e_remote_get_font_cache_cb(E_DBus_Object *obj, DBusMessage *msg)
{
   DBusMessageIter iter;
   DBusMessage *reply;
   int value;

   value = e_config->font_cache;
   
   reply = dbus_message_new_method_return(msg);
   dbus_message_iter_init_append(reply, &iter);
   dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &value);

   return reply;
}

static DBusMessage* 
_e_remote_set_font_cache_cb(E_DBus_Object *obj, DBusMessage *msg)
{
   DBusMessageIter iter;
   int cache; 

   dbus_message_iter_init(msg, &iter);
   dbus_message_iter_get_basic(&iter, &cache);

   e_config->font_cache = cache * 1024;
   //E_CONFIG_LIMIT(e_config->font_cache, 0, 32 * 1024);
   e_canvas_recache();
   e_config_save_queue();
   return dbus_message_new_method_return(msg);
}

static DBusMessage* 
_e_remote_get_image_cache_cb(E_DBus_Object *obj, DBusMessage *msg)
{
   DBusMessageIter iter;
   DBusMessage *reply;
   double value;

   value = e_config->image_cache;
   
   reply = dbus_message_new_method_return(msg);
   dbus_message_iter_init_append(reply, &iter);
   dbus_message_iter_append_basic(&iter, DBUS_TYPE_DOUBLE, &value);

   return reply;
}

static DBusMessage* 
_e_remote_set_image_cache_cb(E_DBus_Object *obj, DBusMessage *msg)
{
   DBusMessageIter iter;
   double cache; 

   dbus_message_iter_init(msg, &iter);
   dbus_message_iter_get_basic(&iter, &cache);

   e_config->image_cache = cache;
   E_CONFIG_LIMIT(e_config->image_cache, 0, 256 * 1024);
   e_canvas_recache();
   e_config_save_queue();
   return dbus_message_new_method_return(msg);
}

static DBusMessage* 
_e_remote_get_edje_cache_cb(E_DBus_Object *obj, DBusMessage *msg)
{
   DBusMessageIter iter;
   DBusMessage *reply;
   int value;

   value = e_config->edje_cache;
   
   reply = dbus_message_new_method_return(msg);
   dbus_message_iter_init_append(reply, &iter);
   dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &value);

   return reply;
}

static DBusMessage* 
_e_remote_set_edje_cache_cb(E_DBus_Object *obj, DBusMessage *msg)
{
   DBusMessageIter iter;
   int cache; 

   dbus_message_iter_init(msg, &iter);
   dbus_message_iter_get_basic(&iter, &cache);

   e_config->edje_cache = cache;
   E_CONFIG_LIMIT(e_config->edje_cache, 0, 256);
   e_canvas_recache();
   e_config_save_queue();
   return dbus_message_new_method_return(msg);
}

static DBusMessage* 
_e_remote_get_edje_c_cache_cb(E_DBus_Object *obj, DBusMessage *msg)
{
   DBusMessageIter iter;
   DBusMessage *reply;
   int value;

   value = e_config->edje_collection_cache;
   
   reply = dbus_message_new_method_return(msg);
   dbus_message_iter_init_append(reply, &iter);
   dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &value);

   return reply;
}

static DBusMessage* 
_e_remote_set_edje_c_cache_cb(E_DBus_Object *obj, DBusMessage *msg)
{
   DBusMessageIter iter;
   int cache; 

   dbus_message_iter_init(msg, &iter);
   dbus_message_iter_get_basic(&iter, &cache);

   e_config->edje_collection_cache = cache;
   E_CONFIG_LIMIT(e_config->edje_collection_cache, 0, 512);
   e_canvas_recache();
   e_config_save_queue();
   return dbus_message_new_method_return(msg);
}

static DBusMessage* 
_e_remote_core_execute_cb(E_DBus_Object *obj, DBusMessage *msg)
{
   DBusMessageIter iter;
   DBusMessage *reply;
   E_Config_Theme *ect;
   const char *command;
   
   dbus_message_iter_init(msg, &iter);
   dbus_message_iter_get_basic(&iter, &command);
   
   ecore_exe_run(command,NULL);

   reply = dbus_message_new_method_return(msg);
   dbus_message_iter_init_append(reply, &iter);
   dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &command);

   return reply;
}

static DBusMessage* 
_e_remote_list_user_theme_cb(E_DBus_Object *obj, DBusMessage *msg)
{
   Eina_List *l,*list;
   DBusMessage *reply;
   DBusMessageIter iter;
   DBusMessageIter arr;
   char buf[1024], *files;
   
   snprintf(buf, sizeof(buf), "%s/themes/", e_user_dir_get());
   list = ecore_file_ls(buf);
	
   reply = dbus_message_new_method_return(msg);
   dbus_message_iter_init_append(reply, &iter);
   dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY, "s", &arr);
   
   EINA_LIST_FOREACH(list, l, files)
     {
	dbus_message_iter_append_basic(&arr, DBUS_TYPE_STRING, &files);
     }
   dbus_message_iter_close_container(&iter, &arr);

   return reply;
}

static DBusMessage* 
_e_remote_theme_add_cb(E_DBus_Object *obj, DBusMessage *msg)
{
   DBusMessageIter iter;
   char *file,buf[1024]; 
   
   dbus_message_iter_init(msg, &iter);
   dbus_message_iter_get_basic(&iter, &file);
   snprintf(buf, sizeof(buf), "%s/themes/%s", e_user_dir_get(), ecore_file_file_get(file));
   ecore_file_cp(file,buf);

   return dbus_message_new_method_return(msg);
}

static DBusMessage* 
_e_remote_theme_del_cb(E_DBus_Object *obj, DBusMessage *msg)
{
   DBusMessageIter iter;
   char *file,buf[1024]; 
   
   dbus_message_iter_init(msg, &iter);
   dbus_message_iter_get_basic(&iter, &file);
   snprintf(buf, sizeof(buf), "%s/themes/%s", e_user_dir_get(), ecore_file_file_get(file));
   ecore_file_unlink(buf);

   return dbus_message_new_method_return(msg);
}

static DBusMessage* 
_e_remote_set_this_screen_wall_cb(E_DBus_Object *obj, DBusMessage *msg)
{
   DBusMessageIter iter;
   char *bg; 
   int container, _zone, desk_x, desk_y;
   Eina_List *fl = NULL, *l;
   E_Zone *z;
   E_Container *con;
  
   dbus_message_iter_init(msg, &iter);
   dbus_message_iter_get_basic(&iter, &bg);
   
   con = e_container_current_get(e_manager_current_get());
   z = e_util_zone_current_get(con->manager);

	for (l = e_config->desktop_backgrounds; l; l = l->next)
	{
		E_Config_Desktop_Background *cfbg;
		cfbg = l->data;
		if ((cfbg->container == z->container->num) && (cfbg->zone == z->id))
			fl = eina_list_append(fl, cfbg);
       }
	while (fl)
	{
		E_Config_Desktop_Background *cfbg;
		cfbg = fl->data;
		e_bg_del(cfbg->container, cfbg->zone, cfbg->desk_x, 
				cfbg->desk_y);
		fl = eina_list_remove_list(fl, fl);
       }
	e_bg_add(z->container->num, z->id, -1, -1, bg);
	e_bg_update();
	e_config_save_queue();
       
   return dbus_message_new_method_return(msg);
}

static DBusMessage* 
_e_remote_config_get_wall_cb(E_DBus_Object *obj, DBusMessage *msg)
{
	DBusMessageIter iter;
	DBusMessage *reply;
	char *config;
	E_Container *con;
	E_Zone *z;
	E_Desk *d;
	E_Config_Desktop_Background *cfbg;
	
	con = e_container_current_get(e_manager_current_get());
	z = e_util_zone_current_get(con->manager);
	d = e_desk_current_get(z);
	
	cfbg = e_bg_config_get(con->num, z->id, d->x, d->y);
	
	config = "ALL_DESKTOPS";
	
	if (cfbg)
	{
		if (cfbg->container >= 0 && cfbg->zone >= 0)
		{
			if (cfbg->desk_x >= 0 && cfbg->desk_y >= 0)
				config = "THIS_DESKTOP";
			else
				config = "THIS_SCREEN";
	       }
	}
	
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append(reply, &iter);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &config);
   return reply;
}


/*Resolution*/
/*just a fake timer to wait until we get reply from xrandr core*/
static Eina_Bool 
_error_no_xrandr()
{
	 fprintf(stderr,"ERROR, no xrandr\n");
};
/*Get*/
static DBusMessage* 
_e_remote_get_resolution_cb(E_DBus_Object *obj, DBusMessage *msg)
{
   DBusMessageIter iter;
   DBusMessage *reply;
   E_Manager *man;
   Ecore_X_Randr_Screen_Size size;
   Ecore_X_Randr_Refresh_Rate rate;
   char buf[128],*value; 

   if (!ecore_x_randr_query())
   {
	ecore_timer_add(0.5,_error_no_xrandr,NULL);
	value = "";
   }
   else
   {
	man = e_manager_current_get();
	ecore_x_randr_screen_current_size_get(man->root,&size.width,&size.height,NULL,NULL);
	snprintf(buf, sizeof(buf), "%dx%d",size.width,size.height);
	value = strdup(buf);
   }
	
   reply = dbus_message_new_method_return(msg);
   dbus_message_iter_init_append(reply, &iter);
   dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &value);

   return reply;
}

/*Set*/
static DBusMessage* 
_e_remote_set_resolution_cb(E_DBus_Object *obj, DBusMessage *msg)
{
    DBusMessageIter iter;
    E_Manager *man;
    int index;

    dbus_message_iter_init(msg, &iter);
    dbus_message_iter_get_basic(&iter, &index);
	
    if (!ecore_x_randr_query())
    {
	ecore_timer_add(0.5,_error_no_xrandr,NULL);
    }
    else
    {
	man = e_manager_current_get();
	ecore_x_randr_screen_primary_output_size_set(man->root,index);
    }

return dbus_message_new_method_return(msg);
}

/*List*/
static DBusMessage* 
_e_remote_list_resolution_cb(E_DBus_Object *obj, DBusMessage *msg)
{
   DBusMessageIter iter;
   DBusMessage *reply;
   DBusMessageIter arr;
   E_Manager *man;
   Ecore_X_Randr_Screen_Size_MM *size;
   char buf[128],*value;
   int i,s;
	
    reply = dbus_message_new_method_return(msg);
    dbus_message_iter_init_append(reply, &iter);
    dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY, "s", &arr);
	
    if (!ecore_x_randr_query())
    {
	ecore_timer_add(0.5,_error_no_xrandr,NULL);
    }
    else
    {
	man = e_manager_current_get();
	size = ecore_x_randr_screen_primary_output_sizes_get(man->root, &s);
	
	for (i = 0; i < (s - 1); i++)
	{
	    snprintf(buf, sizeof(buf), "%dx%d %d",size[i].width,size[i].height,i);
	    value = strdup(buf);
	    dbus_message_iter_append_basic(&arr, DBUS_TYPE_STRING, &value);
	    }
	    dbus_message_iter_close_container(&iter, &arr);
    }
    E_FREE(size);
return reply;
}

int e_remote_init()
{
	data =  E_NEW(Remote_Data, 1);
	
	E_DBus_Interface *iface;
	data->conn = e_dbus_bus_get(DBUS_BUS_SESSION);
	
	if (!data->conn)
	{
		printf("WARNING: Cannot get DBUS_BUS_SESSION\n");
	  return 0;
	}
	e_dbus_request_name(data->conn, "org.enlightenment.Remote.service", 0, NULL, NULL);
	data->obj = e_dbus_object_add(data->conn, "/org/enlightenment/Remote/RemoteObject", NULL);
  
	/*Wallpaper*/
	iface = e_dbus_interface_new("org.enlightenment.Remote.Wallpaper");
	if (!iface)
	{
		printf("WARNING: Cannot add org.enlightenment.Remote.Wallpaper interface\n");
	  return 0;
	}
	e_dbus_object_interface_attach(data->obj, iface);
 
	/* Wallpaper methods */
	e_dbus_interface_method_add(iface, "Set_Current_Desktop", "s", "", _e_remote_set_current_desktop_wall_cb);
	e_dbus_interface_method_add(iface, "Set_This_Screen", "s", "", _e_remote_set_this_screen_wall_cb);
	e_dbus_interface_method_add(iface, "Default_Set", "s", "", _e_remote_set_wall_cb);
	e_dbus_interface_method_add(iface, "Default_Get", "", "s", _e_remote_default_get_wall_cb);
	e_dbus_interface_method_add(iface, "Config_Get", "", "i", _e_remote_config_get_wall_cb);
	e_dbus_interface_method_add(iface, "Get", "iiii", "s", _e_remote_get_wall_cb);
	e_dbus_interface_method_add(iface, "Add", "iiiis", "", _e_remote_add_wall_cb);
	e_dbus_interface_method_add(iface, "Del", "iiii", "", _e_remote_del_wall_cb);
     
	/*Intl*/
	iface = e_dbus_interface_new("org.enlightenment.Remote.Intl");
	if (!iface)
	{
		printf("WARNING: Cannot add org.enlightenment.Remote.Intl interface\n");
	  return 0;
	}
	e_dbus_object_interface_attach(data->obj, iface);
 
	/* Intl methods */
	e_dbus_interface_method_add(iface, "Set", "s", "", _e_remote_set_intl_cb);
	e_dbus_interface_method_add(iface, "Get", "", "s", _e_remote_get_intl_cb);
	e_dbus_interface_method_add(iface, "List", "", "s", _e_remote_list_intl_cb);
	
	/*Core*/
	iface = e_dbus_interface_new("org.enlightenment.Remote.Core");
	if (!iface)
	{
		printf("WARNING: Cannot add org.enlightenment.Remote.Core interface\n");
	  return 0;
	}
	e_dbus_object_interface_attach(data->obj, iface);
 
	/* Core methods */
	e_dbus_interface_method_add(iface, "Hibernate", "", "", _e_remote_core_hibernate_cb);
	e_dbus_interface_method_add(iface, "Suspend", "", "", _e_remote_core_suspend_cb);
	e_dbus_interface_method_add(iface, "Logout", "", "", _e_remote_core_logout_cb);
	e_dbus_interface_method_add(iface, "Reboot", "", "", _e_remote_core_reboot_cb);
	e_dbus_interface_method_add(iface, "Halt", "", "", _e_remote_core_halt_cb);
	e_dbus_interface_method_add(iface, "Halt_Now", "", "", _e_remote_core_halt_now_cb);
	e_dbus_interface_method_add(iface, "Lock", "", "", _e_remote_core_lock_cb);
	e_dbus_interface_method_add(iface, "Execute", "s", "", _e_remote_core_execute_cb);
   // FIXME: Save feature not works, at least for me
	e_dbus_interface_method_add(iface, "Save_Config", "", "", _e_remote_core_save_config_cb);
		
	/*Theme*/
	iface = e_dbus_interface_new("org.enlightenment.Remote.Theme");
	if (!iface)
	{
		printf("WARNING: Cannot add org.enlightenment.Remote.Theme interface\n");
	  return 0;
	}
	e_dbus_object_interface_attach(data->obj, iface);
 
	/* Theme methods */
	e_dbus_interface_method_add(iface, "Set", "ss", "", _e_remote_set_theme_cb);
	e_dbus_interface_method_add(iface, "Get", "s", "", _e_remote_get_theme_cb);
	e_dbus_interface_method_add(iface, "List", "", "s", _e_remote_list_theme_cb);
	e_dbus_interface_method_add(iface, "Add", "s", "", _e_remote_theme_add_cb);
	e_dbus_interface_method_add(iface, "Del", "s", "", _e_remote_theme_del_cb);
	e_dbus_interface_method_add(iface, "List_User_Themes", "", "s", _e_remote_list_user_theme_cb);
	
	/*Framerate*/
	iface = e_dbus_interface_new("org.enlightenment.Remote.Framerate");
	if (!iface)
	{
		printf("WARNING: Cannot add org.enlightenment.Remote.Framerate interface\n");
	  return 0;
	}
	e_dbus_object_interface_attach(data->obj, iface);
 
	/* Framerate methods */
	e_dbus_interface_method_add(iface, "Get", "", "s", _e_remote_get_framerate_cb);
	e_dbus_interface_method_add(iface, "Set", "d", "", _e_remote_set_framerate_cb);
	
	/*Scroll Speed*/
	iface = e_dbus_interface_new("org.enlightenment.Remote.Scrollspeed");
	if (!iface)
	{
		printf("WARNING: Cannot add org.enlightenment.Remote.Scrollspeed interface\n");
	  return 0;
	}
	e_dbus_object_interface_attach(data->obj, iface);
 
	/* Scroll Speed methods */
	e_dbus_interface_method_add(iface, "Get", "", "s", _e_remote_get_scrollspeed_cb);
	e_dbus_interface_method_add(iface, "Set", "d", "", _e_remote_set_scrollspeed_cb);
     
	/*Focus Policy*/
	iface = e_dbus_interface_new("org.enlightenment.Remote.Focus");
	if (!iface)
	{
		printf("WARNING: Cannot add org.enlightenment.Remote.Focus interface\n");
	  return 0;
	}
	e_dbus_object_interface_attach(data->obj, iface);
 
	/* Focus Policy methods */
	e_dbus_interface_method_add(iface, "Get", "", "s", _e_remote_get_focus_cb);
	e_dbus_interface_method_add(iface, "Set", "s", "", _e_remote_set_focus_cb);
	
	/*Font Cache*/
	iface = e_dbus_interface_new("org.enlightenment.Remote.Fonts");
	if (!iface)
	{
		printf("WARNING: Cannot add org.enlightenment.Remote.Fonts interface\n");
	  return 0;
	}
	e_dbus_object_interface_attach(data->obj, iface);
 
	/* Font Cache methods */
	e_dbus_interface_method_add(iface, "Cache_Get", "", "i", _e_remote_get_font_cache_cb);
	e_dbus_interface_method_add(iface, "Cache_Set", "i", "", _e_remote_set_font_cache_cb);
	
	/*Image Cache*/
	iface = e_dbus_interface_new("org.enlightenment.Remote.Image");
	if (!iface)
	{
		printf("WARNING: Cannot add org.enlightenment.Remote.Image interface\n");
	  return 0;
	}
	e_dbus_object_interface_attach(data->obj, iface);
 
	/* Image Cache methods */
	e_dbus_interface_method_add(iface, "Cache_Get", "", "i", _e_remote_get_image_cache_cb);
	e_dbus_interface_method_add(iface, "Cache_Set", "d", "", _e_remote_set_image_cache_cb);
	
	/*Edje Cache*/
	iface = e_dbus_interface_new("org.enlightenment.Remote.Edje");
	if (!iface)
	{
		printf("WARNING: Cannot add org.enlightenment.Remote.Edje interface\n");
	  return 0;
	}
	e_dbus_object_interface_attach(data->obj, iface);
 
	/* Edje Cache methods */
	e_dbus_interface_method_add(iface, "Cache_Get", "", "i", _e_remote_get_edje_cache_cb);
	e_dbus_interface_method_add(iface, "Cache_Set", "i", "", _e_remote_set_edje_cache_cb);
	e_dbus_interface_method_add(iface, "Collection_Cache_Get", "", "i", _e_remote_get_edje_c_cache_cb);
	e_dbus_interface_method_add(iface, "Collection_Cache_Set", "i", "", _e_remote_set_edje_c_cache_cb);
	
	
	iface = e_dbus_interface_new("org.enlightenment.Remote.Resolution");
	if (!iface)
	{
		printf("WARNING: Cannot add org.enlightenment.Remote.Edje interface\n");
	  return 0;
	}
	e_dbus_object_interface_attach(data->obj, iface);
 
	/* Edje Cache methods */
	e_dbus_interface_method_add(iface, "Get", "", "s", _e_remote_get_resolution_cb);
	e_dbus_interface_method_add(iface, "Set", "i", "", _e_remote_set_resolution_cb);
	e_dbus_interface_method_add(iface, "List", "", "s", _e_remote_list_resolution_cb);
}
