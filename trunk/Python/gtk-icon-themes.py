#!/usr/bin/env python2
import pygtk
pygtk.require('2.0')
import gtk, readline, commands,os,sys
VERSION="1.0"
EXTRA=False
if len(sys.argv)>1:
	ARG=sys.argv[1]
	if ARG=="-a":
		EXTRA=True
	if ARG=="-v":
		print "verbose mode"
	if ARG=="-V":
		print "Gtk-Icon-Theme version: ",VERSION
		sys.exit()
else:
	sys.stdout=open("/dev/null", 'w')
	sys.stdin=open("/dev/null", 'r')
themelist=[]
go=0
HOME=os.path.expanduser("~")
if os.access(HOME+"/.icons",0)==False:
	os.mkdir(HOME+"/.icons")

if os.access(HOME+"/.gtkrc-2.0",os.X_OK)==False:
	os.system("touch "+HOME+"/.gtkrc-2.0")

class classname(gtk.Window):
	def main(self):
		gtk.main()
	def delete_event(self, widget, event=None, data=None):
		gtk.main_quit()
		return False
	def set(self, button):
		if button.get_label()== "Use as Fallback":
			s = self.treeview.get_selection()
			(ls, iter) = s.get_selected()
			name = ls.get_value(iter, 1)
			type="gtk-fallback-icon-theme"
			lines=[]
			file=open(HOME+"/.gtkrc.temp")
			for line in file:
				if line.strip().startswith("gtk-fallback"):
					line=""
				lines.append(line)
			file=open(HOME+"/.gtkrc.temp","w")
			for line in lines:
				file.write(line)
			file.write("gtk-fallback-icon-theme = \""+name+"\"\n")
			file.close()
			self.fallbacklbl.set_text("Fallback Theme: "+name)
			gtk.rc_parse(HOME+"/.gtkrc.temp")
		elif button.get_label()== "gtk-apply":
			type="gtk-icon-theme-name"
		global nt
		s = self.treeview.get_selection()
		(ls, iter) = s.get_selected()
		if iter is None:
			print "nothing selected"
		else:
			data1 = ls.get_value(iter, 1)
			mainrcfile=open(HOME+"/.gtkrc-2.0")
			found=False
			mainlines=[]
			for line in mainrcfile:
				mainlines.append(line)
				if line.strip().startswith("include"):
					if HOME+"/.gtkrc.mine" in line:
						if os.access(HOME+"/.gtkrc.mine",0)==False:
							print "creating file"
							os.system("touch \""+HOME+"/.gtkrc.mine\"")
						found=True
			if found ==False:
				mainrcfile=open(HOME+"/.gtkrc-2.0","w")
				for line in mainlines:
					mainrcfile.write(line)
				mainrcfile.write("include \""+HOME+"/.gtkrc.mine\"\n")
				mainrcfile.close()
			customfile1=open(HOME+"/.gtkrc.mine")
			linefound=False
			lines=[]
			for line in customfile1:
				if line.strip().startswith(type):
					line=type+" = \""+data1+"\"\n"
					linefound=True
				lines.append(line)
			customfile2=open(HOME+"/.gtkrc.mine","w")
			for line in lines:
				customfile2.write(line)
			if linefound==False:
				customfile2.write(type+" = \""+data1+"\"\n")
			customfile2.close()
			customfile1.close()
		events=gtk.gdk.Event(gtk.gdk.CLIENT_EVENT)
		data=gtk.gdk.atom_intern("_GTK_READ_RCFILES", False)
		events.data_format=8
		events.send_event=True
		events.message_type=data
		events.send_clientmessage_toall()
		
	def temp_set(self,widget):
		global nt,fallback_theme
		nt="\""
		THEME=gtk.IconTheme()
		s = self.treeview.get_selection()
		(ls, iter) = s.get_selected()
		if iter is None:
			print "nothing selected"
		else:
			data1 = ls.get_value(iter, 1)
			nt = nt+data1+"\""
			file=os.path.expanduser("~")+"/.gtkrc.temp"
			if os.access(file,0)==False:
				print "creating file"
				os.system("touch "+file)

			filename=open(str(file))
			fallbackline=""
			for line in filename:
				if line.strip().startswith("gtk-fallback"):
					fallbackline=line
			filename=open(str(file),"w")
			filename.write("gtk-icon-theme-name = " +nt+"\n")
			filename.write(fallbackline)
			THEME.set_custom_theme(nt)
			print nt
			print "#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#"
			filename.close()
			#gtk.rc_reparse_all_for_settings("gtk-icon-theme-name","~/.gtkrc.mine")
			gtk.rc_parse(file)
			THEME=gtk.IconTheme()
			THEME.set_custom_theme(nt)
			print THEME.list_icons()
			self.themelbl.set_text("Theme Name: "+nt)
			
	def __init__(self):
		self.aboutbtn=gtk.Button(stock=gtk.STOCK_ABOUT)
		def abtfunc(x):
			global VERSION,EXTRA
			x=gtk.AboutDialog()
			x.set_version(VERSION)
			x.set_name("GTK-Icon-Themes")
			y=["David Braker (LinuxNIT)\nContact: linuxnit@elivecd.org"]
			x.set_authors(y)
			if os.access("/usr/share/pixmaps/smb-browser.png",0)==True:
				image=gtk.gdk.pixbuf_new_from_file_at_size("/usr/share/pixmaps/smb-browser.png",60,60)
			else:
				image=None
			#x.set_logo(image)
			def close(w, res):
				if res == gtk.RESPONSE_CANCEL:
					w.hide()
			x.connect("response", close)
			x.set_license("This program was designed to assist users in changing GTK2 icon themes.\n\
		Copyright (C) <2007>  <David Braker>\n\
		This program is free software; you can redistribute it and/or modify\n\
		it under the terms of the GNU General Public License as published by\n\
		the Free Software Foundation; either version 2 of the License, or\n\
		(at your option) any later version.\n\
		\n\
		This program is distributed in the hope that it will be useful,\n\
		but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\
		GNU General Public License for more details.\n\
		\n\
		You should have received a copy of the GNU General Public License along\n\
		with this program; if not, write to the Free Software Foundation, Inc.,\n\
		51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.\n\
		\n\
		On a Debian GNU/Linux system you can find a copy of this license in\n\
		`/usr/share/common-licenses/'.")
			x.set_comments("This program was designed to assist users in changing GTK2 icon themes.")
			x.show()
		self.aboutbtn.connect("clicked",abtfunc)
		gtk.Window.__init__(self)
		self.set_title("GTK Icon Themes")
		self.connect("delete_event", self.delete_event)
		self.applybtn = gtk.Button(stock=gtk.STOCK_APPLY)
		self.quitbtn = gtk.Button(stock=gtk.STOCK_QUIT)
		self.fallbackbtn=gtk.Button("Use as Fallback")
		self.fallbacklbl=gtk.Label()
		self.fallbackinfolbl=gtk.Label("\nThe default fallback is \"gnome\".")
		self.themelbl=gtk.Label()
		self.scrolledwindow = gtk.ScrolledWindow()
		self.liststore = gtk.ListStore(int, str)
		self.treeview = gtk.TreeView(self.liststore)
		#self.cell = gtk.CellRendererText()
		self.quitbtn.connect('clicked', self.delete_event)
		self.applybtn.connect('clicked', self.set)
		self.fallbackbtn.connect('clicked', self.set)
		self.treeview.set_search_column(0)
		self.scrolledwindow.add(self.treeview)
		self.treeview.connect("cursor-changed", self.temp_set)

		self.hbox = gtk.HButtonBox()
		self.mainhbox = gtk.HButtonBox()
		self.hbox.pack_start(self.applybtn)
		self.hbox.pack_start(self.fallbackbtn)
		self.hbox.pack_start(self.aboutbtn)
		#self.hbox.pack_start(self.themelbl)
		self.hbox.pack_start(self.quitbtn)
		self.mainbox=gtk.VBox()
		###PREVIEW BOX####
		buttonsbox=gtk.VButtonBox()
		prev_lbl=gtk.Label("Buttons")
		refresh=gtk.Button(stock=gtk.STOCK_REFRESH)
		cancel=gtk.Button(stock=gtk.STOCK_CANCEL)
		add=gtk.Button(stock=gtk.STOCK_ADD)
		rm=gtk.Button(stock=gtk.STOCK_REMOVE)
		back=gtk.Button(stock=gtk.STOCK_GO_BACK)
		forward=gtk.Button(stock=gtk.STOCK_GO_FORWARD)
		openbtn=gtk.Button(stock=gtk.STOCK_OPEN)
		pref=gtk.Button(stock=gtk.STOCK_PREFERENCES)
		save=gtk.Button(stock=gtk.STOCK_SAVE)
		buttonsbox.pack_start(prev_lbl)
		buttonsbox.pack_start(pref)
		buttonsbox.pack_start(openbtn)
		buttonsbox.pack_start(save)
		buttonsbox.pack_start(refresh)
		buttonsbox.pack_start(cancel)
		buttonsbox.pack_start(add)
		buttonsbox.pack_start(rm)
		buttonsbox.pack_start(back)
		buttonsbox.pack_start(forward)
		iconbox=gtk.VButtonBox()
		icon_lbl=gtk.Label("Icons")
		folder= gtk.image_new_from_icon_name("gnome-fs-directory",4)#gtk.Image()
		hdd=gtk.image_new_from_icon_name("gnome-dev-harddisk",4)
		cdrom=gtk.image_new_from_icon_name("gnome-dev-cdrom",4)
		usb2=gtk.image_new_from_icon_name("gnome-dev-harddisk-1394",4)
		usb=gtk.image_new_from_icon_name("gnome-dev-harddisk-usb",4)
		home=gtk.image_new_from_icon_name("gnome-fs-home",4)
		trash=gtk.image_new_from_icon_name("gnome-fs-trash-empty",4)
		desktop=gtk.image_new_from_icon_name("gnome-fs-desktop",4)
		iconbox.pack_start(icon_lbl)
		iconbox.pack_start(desktop)
		iconbox.pack_start(home)
		iconbox.pack_start(trash)
		iconbox.pack_start(hdd)
		iconbox.pack_start(usb2)
		iconbox.pack_start(usb)
		iconbox.pack_start(folder)
		iconbox.pack_start(cdrom)
		appbox=gtk.VButtonBox()
		app_lbl=gtk.Label(" ")
		tar=gtk.Image()
		tar.set_from_icon_name("gnome-mime-application-x-tar",4)
		mp3=gtk.Image()
		mp3.set_from_icon_name("gnome-mime-audio-x-mp3",4)
		mpg=gtk.Image()
		mpg.set_from_icon_name("gnome-mime-video",4)
		smb=gtk.Image()
		smb.set_from_icon_name("gnome-fs-smb",4)
		network=gtk.Image()
		network.set_from_icon_name("gnome-fs-network",4)
		server=gtk.Image()
		server.set_from_icon_name("gnome-fs-server",4)
		ftp=gtk.Image()
		ftp.set_from_icon_name("gnome-fs-ftp",4)
		appbox.pack_start(app_lbl)
		appbox.pack_start(ftp)
		appbox.pack_start(smb)
		appbox.pack_start(network)
		appbox.pack_start(server)
		appbox.pack_start(tar)
		appbox.pack_start(mpg)
		appbox.pack_start(mp3)
		frame=gtk.Frame("Preview")
		frame.set_label_align(0,0)
		###END PREVIEW BOX###
		self.mainvbox=gtk.VBox()
		self.mainvbox.pack_start(self.mainhbox)
		self.mainvbox.pack_start(self.themelbl)
		self.mainvbox.pack_start(self.fallbacklbl)
		self.mainvbox.pack_start(self.fallbackinfolbl)
		hsep=gtk.HSeparator()
		vpane=gtk.HPaned()
		vpane.pack1(self.scrolledwindow)
		vpane.pack2(frame)
		vpane.set_position(130)
		#self.mainhbox.pack_start(frame)
		frame.add(self.mainvbox)
		self.mainhbox.pack_start(buttonsbox)
		self.mainhbox.pack_start(iconbox)
		self.mainhbox.pack_start(appbox)
		print EXTRA,"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"

		self.mainbox.pack_start(vpane)
		self.mainbox.pack_start(hsep,False,False,5)
		self.mainbox.pack_start(self.hbox)
		self.add(self.mainbox)
		self.show_all()
		self.set_border_width(10)
		gtkrcmine=os.path.expanduser("~")+"/.gtkrc.mine"
		dirs=[]
		usr_dir=os.path.expanduser("~")+"/.icons/"
		sys_dir="/usr/share/icons/"
		print "#############################################"
		for item in os.listdir(sys_dir):
			print item+ " in sys dir"
			if os.path.isdir(sys_dir+item):
				if item not in dirs:
					if "index.theme" in os.listdir(sys_dir+item):
						dirs.append(item)
		print "user dir ="+usr_dir
		for item in os.listdir(usr_dir):
			print "usr item ="+item
			if os.path.isdir(usr_dir+item):
				if item not in dirs:
					if "index.theme" in os.listdir(usr_dir+item):
						dirs.append(item)

		for x in dirs:
			if x in ["default","hicolor"]:
				num=dirs.index(x)
				del dirs[num]
		print dirs	

		dirs.sort()
		for theme in dirs:
			themelist.append(theme)
		y=0
		for item in themelist:
			y=y+1
			iter = self.liststore.append( (y, item,) )
		fallback_theme=""
		cur_theme=""
		if os.access(gtkrcmine,0):
			mine=open(gtkrcmine,"r")
			for line in mine:
				if line.startswith("gtk-icon-theme"):
					cur_theme=line.split("=")[1].strip().strip('"')
					print "Current theme is ",cur_theme
				elif line.startswith("gtk-fallback"):
					fallback_theme=line.split("=")[1].strip().strip('"')
					print "Fallback theme = "+fallback_theme
		self.themelbl.set_text("Theme Name: "+cur_theme)
		self.fallbacklbl.set_text("Fallback Theme: "+fallback_theme)
		r = gtk.CellRendererText()
		self.treeview.insert_column_with_attributes(-1, "Theme Name", r, text=1)
		self.treeview.set_model(self.liststore)
		for icon in [ cdrom, folder,usb,usb2,hdd,trash,home,desktop,mp3,mpg,tar,server,network,smb,ftp]:
			icon.set_pixel_size(40)
		if EXTRA==True:
			import icon_themes
			#self.mainhbox.pack_start(icon_themes.VBOX)
			icon_themes.WINDOW()
def popup():
	global cmd_output2, go
	dialog = gtk.Dialog("Warning!", None,
		gtk.DIALOG_MODAL,
		(gtk.STOCK_CANCEL, 0,
		gtk.STOCK_OK, 1))
	label = gtk.Label("We have found that gnome-settings-daemon \n is currently running. In order for your settings \n to take effect we must close this daemon. \n Do you wish to close it?")
	dialog.vbox.pack_start(label,  2)
	label.show()
	dialog.set_position(gtk.WIN_POS_CENTER_ALWAYS)
	dialog.show_all()
	res=dialog.run()
	if res == 1:
		commands.getstatusoutput("kill "+cmd_output2)
		go=1
		dialog.hide()
		dialog.destroy()	
(status2, cmd_output2)=commands.getstatusoutput("pgrep gnome-settings-")
if cmd_output2:
	popup()
else:
	go =1
if go ==1:
	classname().main()
