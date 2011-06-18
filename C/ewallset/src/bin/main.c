#include <Elementary.h>

#define IMPORT_STRETCH 0
#define IMPORT_TILE 1
#define IMPORT_CENTER 2
#define IMPORT_SCALE_ASPECT_IN 3
#define IMPORT_SCALE_ASPECT_OUT 4
#define IMPORT_SCALE_ASPECT_IN_TILE 5
#define BY_HEIGHT 1
#define BY_WIDTH 0

typedef struct _Ewallset Ewallset;
struct _Ewallset
{
	const char *image_file;
	int sel;
	int desk;
	double quality;
	Eina_Bool quit_bool;
	E_DBus_Connection *conn;
	const char *value;
	Evas_Object *rdg,*win;
	const char *edj;
	char tmpn[32];
	int w;
	int h;
};
int failures;
int successes;

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
		
		char *val;
		dbus_message_iter_get_basic (iter, &val);
		e->value = eina_stringshare_add(val);
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
_message(E_DBus_Connection *conn, char *obj, char *name, char *image)
{
	DBusMessage *msg;
	DBusMessageIter iter;
	
	msg = dbus_message_new_method_call(
		"org.enlightenment.wm.service",
		"/org/enlightenment/Remote/RemoteObject",
		obj,name);
	
	if( image != NULL)
	{
		dbus_message_iter_init_append(msg, &iter);
		dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &image);
	}
	e_dbus_message_send(conn, msg, cb_reply, 10, NULL);
	dbus_message_unref(msg);
}

int ewallset_end()
{
	unlink(e->tmpn);
	eina_stringshare_del(e->value);
	e_dbus_connection_close(e->conn);
	free(e);
	elm_exit();
}

Eina_Bool set_bg_timer(void *data)
{
	if(ecore_file_exists(e->edj))
	{
		//printf("Filesize: %d \n",ecore_file_size(e->edj));
		if(successes++ > 2)
		{
			char *set_bg = strdup(e->edj);
			if (e->desk == 0)
				_message(e->conn,"org.enlightenment.Remote.Wallpaper","Default_Set",set_bg);
			else if (e->desk == 1)
				_message(e->conn,"org.enlightenment.Remote.Wallpaper","Set_Current_Desktop",set_bg);
			else if (e->desk == 2)
				_message(e->conn,"org.enlightenment.Remote.Wallpaper","Set_This_Screen",set_bg);
			ewallset_end();
		}
	}
	//end timer if it fails for more than 40 seconds
	if(failures++ > 400)
		return 0;
	//printf("Failures: %d\n",failures);
	return 1;
}

int get_correct_size(int size, int w, int h,int *tw, int *th,int cord)
{
	float aspect;
	aspect = (float)w/h;
	//printf("aspect: %1.2f \n",aspect);
	
	if (cord == BY_WIDTH)
	{
		if(h > size)
		{
			h = size;
			w = (h*aspect);
//		printf("IMAGE_0: %dx%d \n",w,h);
		}
	}
	else if ( cord == BY_HEIGHT)
	{
		if(w > size)
		{
			w = size;
			h = (w/aspect);
//		printf("IMAGE_1: %dx%d \n",w,h);
		}
	}
	
	*tw = w;
	*th = h;
return 0;
}

static void
usr_click_ok(void *data, Evas_Object *obj, void *event_info)
{	
	evas_object_del(e->win);
	FILE *f;
	int fd,sw,sh;
	char enc[128];
	char *path, *bs_name, cmd[1024],bg_file[1024],set_bg[1024], *home;
	const char *image_file;
	home = getenv("HOME");
	
	path = ecore_file_dir_get(e->image_file);
	image_file = ecore_file_file_get(e->image_file);
	bs_name = ecore_file_strip_ext(image_file);
	
	get_correct_size(900,e->w,e->h,&sw,&sh,BY_WIDTH);
	
	if ( e->quality > 99.5 )
		snprintf(enc, sizeof(enc), "COMP");
	else
		snprintf(enc, sizeof(enc), "LOSSY %1.0f", e->quality);
	
	//printf ("ENC:%s \n",enc);
	strcpy(e->tmpn, "/tmp/e_bgdlg_new.edc-tmp-XXXXXX");
	fd = mkstemp(e->tmpn);
	close(fd);
	
	f = fopen(e->tmpn, "w");
	switch (e->sel)
	{
		case IMPORT_STRETCH:
			fprintf(f,"//Created by PrinceAMD\n"
			"images { image: \"%s\" %s; }\n"
			"collections {\n"
			"group { name: \"e/desktop/background\";\n"
			"data { item: \"style\" \"0\"; }\n"
			"max: %i %i;\n"
			"parts {\n"
			"part { name: \"bg\"; mouse_events: 0;\n"
			"description { state: \"default\" 0.0;\n"
			"image { normal: \"%s\"; }\n"
			"} } } } }\n",image_file,enc,e->w,e->h,image_file);
		break;
		case IMPORT_TILE:
			fprintf(f,"//Created by PrinceAMD\n"
			"images { image: \"%s\" %s; }\n"
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
			"} } } } } } }\n",image_file,enc,e->w,e->h,image_file,e->w,e->h);
		break;
		case IMPORT_CENTER:
			fprintf(f,"//Created by PrinceAMD\n"
			"images { image: \"%s\" %s; }\n"
			"collections {\n"
			"group { name: \"e/desktop/background\";\n"
			"data { item: \"style\" \"2\"; }\n"
			"max: %i %i;\n"
			"parts {\n"
			"part { name: \"bg\"; mouse_events: 0;\n"
			"description { state: \"default\" 0.0;\n"
			"min: %i %i; max: %i %i;\n"
			"image { normal: \"%s\"; }\n"
			"} } } } }\n",image_file,enc,e->w,e->h,e->w,e->h,e->w,e->h,image_file);
		break;
		case IMPORT_SCALE_ASPECT_IN:
			fprintf(f,"//Created by PrinceAMD\n"
			"images { image: \"%s\" %s; }\n"
			"collections {\n"
			"group { name: \"e/desktop/background\";\n"
			"data { item: \"style\" \"3\"; }\n"
			"max: %i %i;\n"
			"parts {\n"
			"part { name: \"bg\"; mouse_events: 0;\n"
			"description { state: \"default\" 0.0;\n"
			"aspect: %1.9f %1.9f; aspect_preference: BOTH;\n"
			"image { normal: \"%s\"; }\n"
			"} } } } }\n",image_file,enc,e->w,e->h,(double)e->w / (double)e->h,(double)e->w / (double)e->h,image_file);
		break;
		case IMPORT_SCALE_ASPECT_OUT:
			fprintf(f,"//Created by PrinceAMD\n"
			"images { image: \"%s\" %s; }\n"
			"collections {\n"
			"group { name: \"e/desktop/background\";\n"
			"data { item: \"style\" \"4\"; }\n"
			"max: %i %i;\n"
			"parts {\n"
			"part { name: \"bg\"; mouse_events: 0;\n"
			"description { state: \"default\" 0.0;\n"
			"image { normal: \"%s\"; }\n"
			"aspect: %1.9f %1.9f; aspect_preference: NONE;\n"
			"} } } } }\n",image_file,enc,e->w,e->h,image_file,(double)e->w / (double)e->h,(double)e->w / (double)e->h);
		break;
		case IMPORT_SCALE_ASPECT_IN_TILE:
			fprintf(f,"//Created by PrinceAMD\n"
			"images { image: \"%s\" %s; }\n"
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
			"} } } } } } }\n",image_file,enc,e->w,e->h,image_file,sw,sh);
		break;
		default:
			/*seriously ?*/
		break;
	}
	fclose(f);
	
	if (strcmp(path, image_file))
	{
		snprintf(cmd, sizeof(cmd), "cd \"%s\" && edje_cc  %s $HOME/.e/e/backgrounds/\"%s\".edj",path,e->tmpn,bs_name);
	}
	else
	{
		snprintf(cmd, sizeof(cmd), "edje_cc  %s $HOME/.e/e/backgrounds/\"%s\".edj",e->tmpn,bs_name);
	}
	snprintf(set_bg, sizeof(set_bg), "%s/.e/e/backgrounds/%s.edj",home,bs_name);
	
	e->edj = eina_stringshare_add(set_bg);
	Ecore_Exe *exe;
	exe = ecore_exe_run(cmd, NULL);
	ecore_timer_add(0.1,set_bg_timer,NULL);
	//need code here to end the window session.
	
}

static void
usr_click_cancel(void *data, Evas_Object *obj, void *event_info)
{
	elm_exit();
}

Eina_Bool get_config_timer(void *data)
{
	if(e->value == NULL)
	{
		return 1;
	}
	else
	{
		//printf("e->value = %s\n",e->value);
		if(!strcmp(e->value, "ALL_DESKTOPS"))	
		e->desk = 0;
		else if(!strcmp(e->value, "THIS_DESKTOP"))	
		e->desk = 1;
		else if(!strcmp(e->value, "THIS_SCREEN"))	
		e->desk = 2;
		elm_radio_value_set(e->rdg, e->desk);
		return 0;
	}
}

void
slider_change_cb(void *data, Evas_Object *obj, void *event_info )
{
	e->quality = elm_slider_value_get(obj);
	printf("Percent: %1.0f\n",e->quality);
}


int
ewallset_main()
{
	Evas_Object *win, *bg, *bx, *bx2, *bx3, *bt, *ic, *image , *frame, *rd, *rdg,*sl;
	int tw,th;
	char buf[1024];
	
	e->win = elm_win_add(NULL, "Ewallset", ELM_WIN_BASIC);
	elm_win_autodel_set(e->win, 1);
	evas_object_focus_set(e->win, 1);
	evas_object_smart_callback_add(e->win, "delete,request", usr_click_cancel, NULL);
	
	bg = elm_bg_add(e->win);
	elm_win_resize_object_add(e->win, bg);
	evas_object_size_hint_weight_set(bg, 1.0, 1.0);
	evas_object_show(bg);
	
	bx = elm_box_add(e->win);
	evas_object_size_hint_weight_set(bx, 1.0, 1.0);
	elm_win_resize_object_add(e->win,bx);
	evas_object_show(bx);
	
	image = elm_image_add(bx);
	elm_image_file_set(image, e->image_file, NULL);
	evas_object_size_hint_weight_set(image, 1.0, 1.0);
        evas_object_size_hint_align_set(image, -1.0, -1.0);
	elm_image_object_size_get(image,&e->w,&e->h);
	get_correct_size(410,e->w,e->h,&tw,&th,BY_HEIGHT);
	printf("Res:%dx%d \n",e->w,e->h);

	evas_object_size_hint_min_set(image, tw, th);
	elm_box_pack_end(bx, image);
	evas_object_show(image);
	
	snprintf(buf, sizeof(buf), "Ewallset - %s", ecore_file_file_get(e->image_file));
	elm_win_title_set(e->win, buf);

	e->quality = 90;
	sl = elm_slider_add(e->win);
	elm_slider_indicator_show_set(sl, EINA_FALSE);
	elm_slider_label_set(sl, "Quality");
	elm_slider_unit_format_set(sl, "%1.0f Percent");
	elm_slider_span_size_set(sl, 250);
	elm_slider_min_max_set(sl, 10, 100);
	elm_slider_value_set(sl, e->quality);
	evas_object_smart_callback_add(sl, "delay,changed", slider_change_cb, NULL);
	evas_object_size_hint_align_set(sl, 0.5, 0.5);
	evas_object_size_hint_weight_set(sl, 0.0, 0.0);
	elm_box_pack_end(bx, sl);
	evas_object_show(sl);

	frame = elm_frame_add(e->win);
	elm_frame_label_set(frame,"Where to place this Wallpaper");
	elm_object_style_set(frame,"outdent_bottom");
	elm_box_pack_end(bx, frame);
	evas_object_show(frame);
	
	bx3 = elm_box_add(e->win);
	elm_box_horizontal_set(bx3, 1);
	elm_frame_content_set(frame,bx3);
	
	rd = elm_radio_add(e->win);
	elm_radio_state_value_set(rd, 0);
	elm_radio_label_set(rd, "All Desktops");
	evas_object_smart_callback_add(rd, "changed", desk_changed,e->rdg);
	elm_box_pack_end(bx3, rd);
	evas_object_show(rd);
	e->rdg = rd;
	
	rd = elm_radio_add(e->win);
	elm_radio_state_value_set(rd, 1);
	elm_radio_group_add(rd, e->rdg);
	elm_radio_label_set(rd, "This Desktop");
	evas_object_smart_callback_add(rd, "changed", desk_changed,e->rdg);
	elm_box_pack_end(bx3, rd);
	evas_object_show(rd);
	
	rd = elm_radio_add(e->win);
	elm_radio_state_value_set(rd, 2);
	elm_radio_group_add(rd, e->rdg);
	elm_radio_label_set(rd, "This Screen");
	evas_object_smart_callback_add(rd, "changed", desk_changed,e->rdg);
	elm_box_pack_end(bx3, rd);
	evas_object_show(rd);
	
	ecore_timer_add(0.1,get_config_timer,NULL);
	
	
	frame = elm_frame_add(e->win);
	elm_frame_label_set(frame,"Fill and Stretch Options");
	elm_object_style_set(frame,"outdent_top");
	elm_box_pack_end(bx, frame);
	evas_object_show(frame);
	
	bx3 = elm_box_add(e->win);
	elm_box_horizontal_set(bx3, 1);
	//elm_box_homogenous_set(bx3, 1);
	elm_frame_content_set(frame,bx3);
	evas_object_show(bx3);
	
	rd = elm_radio_add(e->win);
	elm_radio_state_value_set(rd, IMPORT_STRETCH);
	elm_radio_label_set(rd, "Stretch");
	evas_object_smart_callback_add(rd, "changed", selection_changed,rdg);
	elm_box_pack_end(bx3, rd);
	evas_object_show(rd);
	rdg = rd;
	
	rd = elm_radio_add(e->win);
	elm_radio_state_value_set(rd, IMPORT_TILE);
	elm_radio_group_add(rd, rdg);
	elm_radio_label_set(rd, "Tile");
	evas_object_smart_callback_add(rd, "changed", selection_changed,rdg);
	elm_box_pack_end(bx3, rd);
	evas_object_show(rd);
	
	rd = elm_radio_add(e->win);
	elm_radio_state_value_set(rd, IMPORT_CENTER);
	elm_radio_group_add(rd, rdg);
	elm_radio_label_set(rd, "Center");
	evas_object_smart_callback_add(rd, "changed", selection_changed,rdg);
	elm_box_pack_end(bx3, rd);
	evas_object_show(rd);
	
	rd = elm_radio_add(e->win);
	elm_radio_state_value_set(rd, IMPORT_SCALE_ASPECT_IN);
	elm_radio_group_add(rd, rdg);
	elm_radio_label_set(rd, "Within");
	evas_object_smart_callback_add(rd, "changed", selection_changed,rdg);
	elm_box_pack_end(bx3, rd);
	evas_object_show(rd);
	
	rd = elm_radio_add(e->win);
	elm_radio_state_value_set(rd, IMPORT_SCALE_ASPECT_OUT);
	elm_radio_group_add(rd, rdg);
	elm_radio_label_set(rd, "Fill");
	evas_object_smart_callback_add(rd, "changed", selection_changed,rdg);
	elm_box_pack_end(bx3, rd);
	evas_object_show(rd);
	
	if(th > tw)
	{
	rd = elm_radio_add(e->win);
	elm_radio_state_value_set(rd, IMPORT_SCALE_ASPECT_IN_TILE);
	elm_radio_group_add(rd, rdg);
	elm_radio_label_set(rd, "Tile-In");
	evas_object_smart_callback_add(rd, "changed", selection_changed,rdg);
	elm_box_pack_end(bx3, rd);
	evas_object_show(rd);
	}
	
	elm_radio_value_set(rdg, 0);

	bx2 = elm_box_add(e->win);
	elm_box_horizontal_set(bx2, 1);
	//elm_box_homogenous_set(bx2, 1);
	elm_box_pack_end(bx, bx2);
	evas_object_show(bx2);
	
	ic = elm_icon_add(e->win);
	elm_icon_file_set(ic, "/usr/share/ewallset/data/dialog-ok.png", NULL);
	bt = elm_button_add(bx2);
	elm_button_label_set(bt, "Continue");
	elm_button_icon_set(bt, ic);
	evas_object_smart_callback_add(bt, "clicked", usr_click_ok, NULL);
	elm_box_pack_end(bx2, bt);
	evas_object_show(bt);
   
	ic = elm_icon_add(e->win);
	elm_icon_file_set(ic, "/usr/share/ewallset/data/dialog-cancel.png", NULL);
	bt = elm_button_add(bx2);
	elm_button_label_set(bt, "Cancel");
	elm_button_icon_set(bt, ic);
	evas_object_smart_callback_add(bt, "clicked", usr_click_cancel, NULL);
	elm_box_pack_end(bx2, bt);
	evas_object_show(bt);
	evas_object_show(e->win);
	//evas_object_del(e->win);
return 0;
}

const Ecore_Getopt general_opts = 
{
	"ewallset",
	NULL,
	"0.5",
	"(C) 2009 - 2011 - PrinceAMD <princeamd.elive@gmail.com>",
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
	_message(e->conn,"org.enlightenment.Remote.Wallpaper","Config_Get",NULL);
	
	ewallset_main();
	elm_run();
	elm_shutdown();
return 0;
}
ELM_MAIN()
