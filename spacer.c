/*
    Copyright (c) 2016 Valters Mednis

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>

#include </opt/deadbeef/include/deadbeef/deadbeef.h>
#include </opt/deadbeef/include/deadbeef/gtkui_api.h>


#define SPACER_WIDGET_TYPE "spacer"

static DB_functions_t *deadbeef;
static ddb_gtkui_t *gtkui_plugin;


static void
print(const char* str) {
    #if GTK_CHECK_VERSION(3, 0, 0)
    printf("\e[1;32mSpacer(gtk3): %s\e[0m\n", str);
    #else
    printf("\e[1;32mSpacer(gtk2): %s\e[0m\n", str);
    #endif
}

typedef struct {
    ddb_gtkui_widget_t base;
    int width;
    int height;
} w_spacer_t;

static void
w_spacer_set_size(w_spacer_t * widget, int width, int height) {
    widget->width = width;
    widget->height = height;

    gtk_widget_set_size_request(widget->base.widget, widget->width, widget->height);
}

static void
w_spacer_save(ddb_gtkui_widget_t *w, char *s, int sz) {
    w_spacer_t * widget = (w_spacer_t *) w;

    //Generate save data
    char savedata[25];

    sprintf(savedata, " %i %i", widget->width, widget->height);

    //Append save data
    strncat(s, savedata, sz);
}

const char *
w_spacer_load(ddb_gtkui_widget_t *w, const char *type, const char *s) {
    w_spacer_t * widget = (w_spacer_t *) w;

    int width, height;

    //Do the reading magic
    sscanf(s, " %i %i", &width, &height);

    w_spacer_set_size(widget, width, height);

    //return a pointer the way api wants it back
    return strchr(s, '{') + 1;
}

typedef struct {
    w_spacer_t * widget;
    GtkWidget * widthentry;
    GtkWidget * heightentry;
} w_spacer_menu_changesize_dialog_userdata_t;

static void
w_spacer_menu_changesize_dialog(GtkWidget * dialog, int response, void * userdata) {
    print("Dialog action performed");
    w_spacer_menu_changesize_dialog_userdata_t * data = userdata;
    w_spacer_t * widget = data->widget;
    GtkWidget * widthentry = data->widthentry;
    GtkWidget * heightentry = data->heightentry;

    int width = gtk_spin_button_get_value_as_int((GtkSpinButton *) widthentry);
    int height = gtk_spin_button_get_value_as_int((GtkSpinButton *) heightentry);

    switch (response) {
        case GTK_RESPONSE_APPLY: {
            w_spacer_set_size(widget, width, height);
        } break;
        case GTK_RESPONSE_OK: {
            w_spacer_set_size(widget, width, height);
            gtk_widget_destroy(dialog);
        } break;
        default: {
            gtk_widget_destroy(dialog);
        } break;
    }
}

static void
w_spacer_menu_changesize(GtkWidget * menuitem, void * userdata) {
    print("Menu item activated");
    w_spacer_t * widget = userdata;

    //Build the dialog
    GtkWidget * dialog = gtk_dialog_new_with_buttons("Change spacer size", (GtkWindow *) gtkui_plugin->get_mainwin(),
                                                      GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                      "Apply", GTK_RESPONSE_APPLY, "Cancel", GTK_RESPONSE_CANCEL, "OK", GTK_RESPONSE_OK,
                                                      NULL);
    GtkWidget * dialogcontent = gtk_dialog_get_content_area((GtkDialog *) dialog);
    gtk_orientable_set_orientation((GtkOrientable *) dialogcontent, GTK_ORIENTATION_VERTICAL);

    #if GTK_CHECK_VERSION(3, 0, 0)
    GtkWidget * widthbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget * heightbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    #else
    GtkWidget * widthbox = gtk_hbox_new(FALSE, 0);
    GtkWidget * heightbox = gtk_hbox_new(FALSE, 0);
    #endif
    gtk_container_add(GTK_CONTAINER(dialogcontent), widthbox);
    gtk_container_add(GTK_CONTAINER(dialogcontent), heightbox);
    gtk_box_set_child_packing((GtkBox *) dialogcontent, widthbox, TRUE, TRUE, 0, GTK_PACK_START);
    gtk_box_set_child_packing((GtkBox *) dialogcontent, heightbox, TRUE, TRUE, 0, GTK_PACK_END);

    GtkWidget * widthlabel = gtk_label_new("Width:");
    GtkWidget * widthentry = gtk_spin_button_new_with_range((double) 0, (double) (8192), (double) 1);
    gtk_container_add(GTK_CONTAINER(widthbox), widthlabel);
    gtk_container_add(GTK_CONTAINER(widthbox), widthentry);
    gtk_box_set_child_packing((GtkBox *) widthbox, widthlabel, TRUE, TRUE, 8, GTK_PACK_START);
    gtk_box_set_child_packing((GtkBox *) widthbox, widthentry, FALSE, FALSE, 8, GTK_PACK_START);
    gtk_label_set_justify((GtkLabel *) widthlabel, GTK_JUSTIFY_LEFT);


    GtkWidget * heightlabel = gtk_label_new("Height:");
    GtkWidget * heightentry = gtk_spin_button_new_with_range((double) 0, (double) (8192), (double) 1);
    gtk_container_add(GTK_CONTAINER(heightbox), heightlabel);
    gtk_container_add(GTK_CONTAINER(heightbox), heightentry);
    gtk_box_set_child_packing((GtkBox *) heightbox, heightlabel, TRUE, TRUE, 8, GTK_PACK_START);
    gtk_box_set_child_packing((GtkBox *) heightbox, heightentry, FALSE, FALSE, 8, GTK_PACK_START);
    gtk_label_set_justify((GtkLabel *) heightlabel, GTK_JUSTIFY_LEFT);

    gtk_spin_button_set_value((GtkSpinButton *) widthentry, (double) widget->width);
    gtk_spin_button_set_value((GtkSpinButton *) heightentry, (double) widget->height);

    gtk_widget_show(dialog);
    gtk_widget_show_all(dialogcontent);

    //Register the event listenr
    w_spacer_menu_changesize_dialog_userdata_t * data = malloc(sizeof(w_spacer_menu_changesize_dialog_userdata_t));
    data->widget = widget;
    data->widthentry = widthentry;
    data->heightentry = heightentry;
    g_signal_connect_after((void *) dialog, "response", G_CALLBACK(w_spacer_menu_changesize_dialog), data);
}

static void
w_spacer_initmenu(ddb_gtkui_widget_t * widget, GtkWidget *menu) {
    GtkWidget * menuitem = gtk_menu_item_new_with_mnemonic("Change spacer size");
    gtk_widget_show(menuitem);
    gtk_container_add(GTK_CONTAINER(menu), menuitem);

    //Register the event handler
    g_signal_connect_after((void *) menuitem, "activate", G_CALLBACK(w_spacer_menu_changesize), widget);
}

static ddb_gtkui_widget_t *
w_spacer_create(void) {
    print("Creating widget");
    w_spacer_t * widget = malloc(sizeof(w_spacer_t));
    memset(widget, 0, sizeof(w_spacer_t));

    widget->base.widget = (GtkWidget *) gtk_event_box_new();
    widget->base.initmenu = w_spacer_initmenu;
    widget->base.save = w_spacer_save;
    widget->base.load = w_spacer_load;

    //The default spacer size
    w_spacer_set_size(widget, 64, 64);

    //Needed so that the design mode would work
    gtkui_plugin->w_override_signals(widget->base.widget, widget);

    return (ddb_gtkui_widget_t *) widget;
}

static int
spacer_connect (void) {
    print("Connect was called");
	  gtkui_plugin = (ddb_gtkui_t *)deadbeef->plug_get_for_id(DDB_GTKUI_PLUGIN_ID);

    //If GTK UI plugin isn't loaded or uses wrong gtk version
	  if(!gtkui_plugin) {
        print("GTK UI not loaded or wrong version (i.e. gtk2 for gtk3 and vice versa)!");
	      return -1;
	  }
	  gtkui_plugin->w_reg_widget("Spacer", 0, w_spacer_create, SPACER_WIDGET_TYPE, NULL);
	  return 0;
}

static int
spacer_disconnect (void) {
    print("Disconnect called!");

    if(gtkui_plugin) {
        //Don't do this if gtkui plugin never existed
        gtkui_plugin->w_unreg_widget(SPACER_WIDGET_TYPE);
    }
    gtkui_plugin = NULL;
    return 0;
}

DB_misc_t plugin = {
    .plugin.api_vmajor = DB_API_VERSION_MAJOR,
    .plugin.api_vminor = DB_API_VERSION_MINOR,
    .plugin.type = DB_PLUGIN_MISC,
    .plugin.version_major = 1,
    .plugin.version_minor = 0,
    #if GTK_CHECK_VERSION(3, 0, 0)
    .plugin.id = "spacer-gtk3",
    .plugin.name ="Spacer (gtk3)",
    #else
    .plugin.id = "spacer-gtk2",
    .plugin.name ="Spacer (gtk2)",
    #endif
    .plugin.descr = "Provides GUI element that acts like a spacer so you can create that perfect layout.\n" \
                    "The size of the spacer can be changed in design mode.\n",
    .plugin.copyright = "Copyright (c) 2016 Valters Mednis\n\n" \
                        "Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the \"Software\"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:\n\n" \
                        "The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.\n\n"
                        "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.",
    .plugin.website = "https://github.com/vmednis/ddb_spacer",
    .plugin.connect = spacer_connect,
    .plugin.disconnect = spacer_disconnect
};

extern DB_plugin_t *
#if GTK_CHECK_VERSION(3, 0, 0)
ddb_spacer_gtk3_load (DB_functions_t *ddb) { //if using gtk3 or later
#else
ddb_spacer_gtk2_load (DB_functions_t *ddb) { //Otherwise we are obviously on gtk2
#endif
    print("Load was called");
    deadbeef = ddb;
    return DB_PLUGIN(&plugin);
}
