#include <gtk/gtk.h>
#include <locale.h>
#include <libintl.h>

void on_window1_destroy(GtkWidget *window, gpointer *data) {
  gtk_main_quit();
}

int main (int argc, char *argv[]) {
  // Set the current local to default
  setlocale(LC_ALL, "");

  // bindtextdomain(DOMAINNAME, DIRNAME)
  //
  // Specify that the DOMAINNAME message catalog
  // will be found in DIRNAME rather than in
  // the system locale data base.
  bindtextdomain("gmultilang", "/usr/share/locale");

  // textdomain(DOMAINNAME)
  //
  // Set the current default message catalog to DOMAINNAME.
  textdomain("gmultilang");

  GtkBuilder *builder;
  GtkWidget *window;

  gtk_init(&argc, &argv);
  builder = gtk_builder_new();
  gtk_builder_add_from_file(builder, "gmultilang.ui", NULL);

  window = GTK_WIDGET(gtk_builder_get_object(builder, "window1"));

  gtk_builder_connect_signals(builder, NULL);
  g_object_unref(G_OBJECT(builder));

  gtk_widget_show_all(window);
  gtk_main();

  return 0;
}
