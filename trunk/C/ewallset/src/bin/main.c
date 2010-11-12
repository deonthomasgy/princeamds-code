#include <Elementary.h>

typedef struct _Ewallset Ewallset;
struct _Ewallset
{
	const char *image_file;
	int sel;
	int desk;
	Eina_Bool quit_bool;
	E_DBus_Connection *conn;
	const char *value;
	int w;
	int h;
};

Ewallset *e;

static void
selection_changed(void *data, Evas_Object *obj, void *event_info)
{
	e->sel = elm_radio_value_get(obj);
}

static void
desk_changed(void *data, Evas_Object *obj, void *event_info)
{
	e->desk = elm_radio_value_get(obj);
}

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
				e->value = eina_stringshare_add(val);
				break;
			}
		}
	}
	while (dbus_message_iter_next (iter));
}

void
cb_reply(void *data, DBusMessage *reply, DBusError *error)
{
	DBusMessageIter subiter;
	DBusMessageIter iter;
	dbus_message_iter_init (reply, &iter);
	print_iter(&iter,1);
}

static void 
_message(E_DBus_Connection *conn, char *name, char *image)
{
	DBusMessage *msg;
	DBusMessageIter iter;
	
	msg = dbus_message_new_method_call(
		"org.enlightenment.Remote.service",
		"/org/enlightenment/Remote/RemoteObject",
		"org.enlightenment.Remote.Wallpaper",name);
	dbus_message_iter_init_append(msg, &iter);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &image);
	e_dbus_message_send(conn, msg, cb_reply, 10, NULL);
	dbus_message_unref(msg);
}

static void 
_config_get(E_DBus_Connection *conn, char *name)
{
	DBusMessage *msg;
	DBusMessageIter iter;
	
	msg = dbus_message_new_method_call(
		"org.enlightenment.Remote.service",
		"/org/enlightenment/Remote/RemoteObject",
		"org.enlightenment.Remote.Wallpaper",name);
	e_dbus_message_send(conn, msg, cb_reply, 10, NULL);
	dbus_message_unref(msg);
}

int ewallset_end()
{
	eina_stringshare_del(e->value);
	e_dbus_connection_close(e->conn);
	free(e);
	elm_exit();
}

static void
usr_click_ok(void *data, Evas_Object *obj, void *event_info)
{	
	FILE *f;
	int fd;
	char tmpn[32];
	char *path, *bs_name, cmd[1024],bg_file[1024],set_bg[1024], *home;
	const char *image_file;
	home = getenv("HOME");
	
	path = ecore_file_dir_get(e->image_file);
	image_file = ecore_file_file_get(e->image_file);
	bs_name = ecore_file_strip_ext(image_file);
	
	strcpy(tmpn, "/tmp/e_bgdlg_new.edc-tmp-XXXXXX");
	fd = mkstemp(tmpn);
	close(fd);
	f = fopen(tmpn, "w");
	
	if ( e->sel == 0 ) 
	{
		fprintf(f,"//Created by PrinceAMD\n"
			"images { image: \"%s\" LOSSY 90; }\n"
			"collections {\n"
			"group { name: \"e/desktop/background\";\n"
			"data { item: \"style\" \"0\"; }\n"
			"max: %i %i;\n"
			"parts {\n"
			"part { name: \"bg\"; mouse_events: 0;\n"
			"description { state: \"default\" 0.0;\n"
			"image { normal: \"%s\"; }\n"
			"} } } } }\n",image_file,e->w,e->h,image_file);
		fclose(f);
	}
	else if ( e->sel == 1 ) 
	{
		fprintf(f,"//Created by PrinceAMD\n"
			"images { image: \"%s\" LOSSY 90; }\n"
			"collections {\n"
			"group { name: \"e/desktop/background\";\n"
			"data { item: \"style\" \"2\"; }\n"
			"max: %i %i;\n"
			"parts {\n"
			"part { name: \"bg\"; mouse_events: 0;\n"
			"description { state: \"default\" 0.0;\n"
			"min: %i %i; max: %i %i;\n"
			"image { normal: \"%s\"; }\n"
			"} } } } }\n",image_file,e->w,e->h,e->w,e->h,e->w,e->h,image_file);
		fclose(f);
	}
	else if ( e->sel == 3 ) 
	{
		fprintf(f,"//Created by PrinceAMD\n"
			"images { image: \"%s\" LOSSY 90; }\n"
			"collections {\n"
			"group { name: \"e/desktop/background\";\n"
			"data { item: \"style\" \"3\"; }\n"
			"max: %i %i;\n"
			"parts {\n"
			"part { name: \"bg\"; mouse_events: 0;\n"
			"description { state: \"default\" 0.0;\n"
			"aspect: %1.9f %1.9f; aspect_preference: BOTH;\n"
			"image { normal: \"%s\"; }\n"
			"} } } } }\n",image_file,e->w,e->h,(double)e->w / (double)e->h,(double)e->w / (double)e->h,image_file);
		fclose(f);
	}
	else if ( e->sel == 2 ) 
	{
		fprintf(f,"//Created by PrinceAMD\n"
			"images { image: \"%s\" LOSSY 90; }\n"
			"collections {\n"
			"group { name: \"e/desktop/background\";\n"
			"data { item: \"style\" \"1\"; }\n"
			"max: %i %i;\n"
			"parts {\n"
			"part { name: \"bg\"; mouse_events: 0;\n"
			"description { state: \"default\" 0.0;\n"
			"image { normal: \"%s\"; }\n"
			"fill { size {\n"
			"relative: 0.0 0.0;\n"
			"offset: %i %i;\n"
			"} } } } } } }\n",image_file,e->w,e->h,image_file,e->w,e->h);
		fclose(f);
	}
	else if ( e->sel == 4 ) 
	{
		fprintf(f,"//Created by PrinceAMD\n"
			"images { image: \"%s\" LOSSY 90; }\n"
			"collections {\n"
			"group { name: \"e/desktop/background\";\n"
			"data { item: \"style\" \"4\"; }\n"
			"max: %i %i;\n"
			"parts {\n"
			"part { name: \"bg\"; mouse_events: 0;\n"
			"description { state: \"default\" 0.0;\n"
			"image { normal: \"%s\"; }\n"
			"aspect: %1.9f %1.9f; aspect_preference: NONE;\n"
			"} } } } }\n",image_file,e->w,e->h,image_file,(double)e->w / (double)e->h,(double)e->w / (double)e->h);
		fclose(f);
	}
	
	if (strcmp(path, image_file))
	{
		snprintf(cmd, sizeof(cmd), "cd \"%s\" && edje_cc  %s $HOME/.e/e/backgrounds/\"%s\".edj",path,tmpn,bs_name);
	}
	else
	{
		snprintf(cmd, sizeof(cmd), "edje_cc  %s $HOME/.e/e/backgrounds/\"%s\".edj",tmpn,bs_name);
	}
	snprintf(set_bg, sizeof(set_bg), "%s/.e/e/backgrounds/%s.edj",home,bs_name);
	
	Ecore_Exe *exe;
	exe = ecore_exe_run(cmd, NULL);
	
	sleep(2);
	unlink(tmpn);
	
	if (e->desk == 0)
	{
		_message(e->conn,"Default_Set",set_bg);
	}
	else if (e->desk == 1)
	{
		_message(e->conn,"Set_Current_Desktop",set_bg);
	}
	else if (e->desk == 2)
	{
		_message(e->conn,"Set_This_Screen",set_bg);
	}
	ewallset_end();
}

static void
usr_click_cancel(void *data, Evas_Object *obj, void *event_info)
{
	ewallset_end();
}

//this function input real image size w/h and return a 512 width thumb with tw/th
int _get_correct_size(int w, int h,int *tw, int *th)
{
	float aspect;
	aspect = (float)w/h;
		
	if(w > 512)
	{
		while(w > 512)
		{
			h = (h-1);
			w = (h*aspect);
		}
	}
	*tw = w;
	*th = h;
return 0;
}

int
ewallset_main(void *data)
{
	Evas_Object *win, *bg, *bx, *bx2, *bx3, *bt, *ic, *image , *frame, *rd, *rdg;
	int w, h,i,tw,th;
	char buf[1024];
	
	win = elm_win_add(NULL, "Ewallset", ELM_WIN_BASIC);
	elm_win_autodel_set(win, 1);
	evas_object_focus_set(win, 1);
	evas_object_smart_callback_add(win, "delete-request", usr_click_cancel, NULL);
	
	bg = elm_bg_add(win);
	elm_win_resize_object_add(win, bg);
	evas_object_size_hint_weight_set(bg, 1.0, 1.0);
	evas_object_show(bg);
	
	bx = elm_box_add(win);
	evas_object_size_hint_weight_set(bx, 1.0, 1.0);
	elm_win_resize_object_add(win,bx);
	evas_object_show(bx);
	
	image = elm_image_add(bx);
	elm_image_file_set(image, e->image_file, NULL);
	evas_object_size_hint_weight_set(image, 1.0, 1.0);
        evas_object_size_hint_align_set(image, -1.0, -1.0);
	elm_image_object_size_get(image,&w,&h);
	_get_correct_size(w,h,&tw,&th);
	evas_object_size_hint_min_set(image, tw, th);
	elm_box_pack_end(bx, image);
	evas_object_show(image);
	
	snprintf(buf, sizeof(buf), "Ewallset - %s | Res:%dx%d", ecore_file_file_get(e->image_file),w,h);
	elm_win_title_set(win, buf);
	
	e->w = w;
	e->h = h;
	
	if(!strcmp(e->value, "ALL_DESKTOPS"))	
		i = 0;
	else if(!strcmp(e->value, "THIS_DESKTOP"))	
		i = 1;
	else if(!strcmp(e->value, "THIS_SCREEN"))	
		i = 2;
	e->desk = i;
	
	frame = elm_frame_add(win);
	elm_frame_label_set(frame,"Where to place this Wallpaper");
	elm_object_style_set(frame,"outdent_bottom");
	elm_box_pack_end(bx, frame);
	evas_object_show(frame);
	
	bx3 = elm_box_add(win);
	elm_box_horizontal_set(bx3, 1);
	elm_frame_content_set(frame,bx3);
	
	rd = elm_radio_add(win);
	elm_radio_state_value_set(rd, 0);
	elm_radio_label_set(rd, "All Desktops");
	evas_object_smart_callback_add(rd, "changed", desk_changed,rdg);
	elm_box_pack_end(bx3, rd);
	evas_object_show(rd);
	rdg = rd;
	
	rd = elm_radio_add(win);
	elm_radio_state_value_set(rd, 1);
	elm_radio_group_add(rd, rdg);
	elm_radio_label_set(rd, "This Desktop");
	evas_object_smart_callback_add(rd, "changed", desk_changed,rdg);
	elm_box_pack_end(bx3, rd);
	evas_object_show(rd);
	
	rd = elm_radio_add(win);
	elm_radio_state_value_set(rd, 2);
	elm_radio_group_add(rd, rdg);
	elm_radio_label_set(rd, "This Screen");
	evas_object_smart_callback_add(rd, "changed", desk_changed,rdg);
	elm_box_pack_end(bx3, rd);
	evas_object_show(rd);
	
	elm_radio_value_set(rdg, i);
	
	frame = elm_frame_add(win);
	elm_frame_label_set(frame,"Fill and Stretch Options");
	elm_object_style_set(frame,"outdent_top");
	elm_box_pack_end(bx, frame);
	evas_object_show(frame);
	
	bx3 = elm_box_add(win);
	elm_box_horizontal_set(bx3, 1);
	elm_box_homogenous_set(bx3, 1);
	elm_frame_content_set(frame,bx3);
	evas_object_show(bx3);
	
	rd = elm_radio_add(win);
	elm_radio_state_value_set(rd, 0);
	elm_radio_label_set(rd, "Stretch");
	evas_object_smart_callback_add(rd, "changed", selection_changed,rdg);
	elm_box_pack_end(bx3, rd);
	evas_object_show(rd);
	rdg = rd;
	
	rd = elm_radio_add(win);
	elm_radio_state_value_set(rd, 1);
	elm_radio_group_add(rd, rdg);
	elm_radio_label_set(rd, "Center");
	evas_object_smart_callback_add(rd, "changed", selection_changed,rdg);
	elm_box_pack_end(bx3, rd);
	evas_object_show(rd);
	
	rd = elm_radio_add(win);
	elm_radio_state_value_set(rd, 2);
	elm_radio_group_add(rd, rdg);
	elm_radio_label_set(rd, "Tile");
	evas_object_smart_callback_add(rd, "changed", selection_changed,rdg);
	elm_box_pack_end(bx3, rd);
	evas_object_show(rd);
	
	rd = elm_radio_add(win);
	elm_radio_state_value_set(rd, 3);
	elm_radio_group_add(rd, rdg);
	elm_radio_label_set(rd, "Within");
	evas_object_smart_callback_add(rd, "changed", selection_changed,rdg);
	elm_box_pack_end(bx3, rd);
	evas_object_show(rd);
	
	rd = elm_radio_add(win);
	elm_radio_state_value_set(rd, 4);
	elm_radio_group_add(rd, rdg);
	elm_radio_label_set(rd, "Fill");
	evas_object_smart_callback_add(rd, "changed", selection_changed,rdg);
	elm_box_pack_end(bx3, rd);
	evas_object_show(rd);
	
	elm_radio_value_set(rdg, 0);

	bx2 = elm_box_add(win);
	elm_box_horizontal_set(bx2, 1);
	elm_box_homogenous_set(bx2, 1);
	elm_box_pack_end(bx, bx2);
	evas_object_show(bx2);
	
	ic = elm_icon_add(win);
	elm_icon_file_set(ic, "/usr/share/ewallset/data/dialog-ok.png", NULL);
	bt = elm_button_add(bx2);
	elm_button_label_set(bt, "Continue");
	elm_button_icon_set(bt, ic);
	evas_object_smart_callback_add(bt, "clicked", usr_click_ok, NULL);
	elm_box_pack_end(bx2, bt);
	evas_object_show(bt);
   
	ic = elm_icon_add(win);
	elm_icon_file_set(ic, "/usr/share/ewallset/data/dialog-cancel.png", NULL);
	bt = elm_button_add(bx2);
	elm_button_label_set(bt, "Cancel");
	elm_button_icon_set(bt, ic);
	evas_object_smart_callback_add(bt, "clicked", usr_click_cancel, NULL);
	elm_box_pack_end(bx2, bt);
	evas_object_show(bt);
	evas_object_show(win);
return 0;
}

const Ecore_Getopt general_opts = 
{
	"ewallset",
	NULL,
	"0.5",
	"(C) 2009 - PrinceAMD <princeamd.elive@gmail.com>",
	"GPL v3 - GNU General Public License",
	"Thunar wallpaper setting program written in Elementary.\n"
	"\n"
	"These are options which you can use \n "
	"Ewallset Version 0.5",
	0,
	{
		ECORE_GETOPT_STORE_STR
		('i', "image", "Image file absolute or relative path"),
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
	e = calloc(1, sizeof(Ewallset));
	int arg_index;
	char *image_file;
	
	Ecore_Getopt_Value general_values[] = {
		ECORE_GETOPT_VALUE_STR(image_file),
		ECORE_GETOPT_VALUE_BOOL(e->quit_bool),
		ECORE_GETOPT_VALUE_BOOL(e->quit_bool),
		ECORE_GETOPT_VALUE_BOOL(e->quit_bool),
		ECORE_GETOPT_VALUE_BOOL(e->quit_bool),
		ECORE_GETOPT_VALUE_NONE
	};

	arg_index = ecore_getopt_parse(&general_opts, general_values, argc, argv);
	    
	if (arg_index <= 1)
	{
		printf("Try `ewallset --help' for more information.\n");
		e->quit_bool = EINA_TRUE;
	}
	
	e->image_file = eina_stringshare_add(image_file);
	
	if (e->quit_bool)
		exit(0);
return EINA_TRUE;
}

EAPI int
elm_main(int argc, char **argv)
{	
	e_dbus_init();
	if (!_args_init(argc, argv))
	{
		fprintf(stderr, "Cannot parse arguments, exiting.\n");
	return 1;
	}
	
	e->conn = e_dbus_bus_get(DBUS_BUS_SESSION);
	_config_get(e->conn,"Config_Get");
	
	ecore_timer_add(0.05,ewallset_main,NULL);
	elm_run();
	elm_shutdown();
return 0;
}
ELM_MAIN()