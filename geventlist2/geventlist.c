#include <gtk/gtk.h>
#include <string.h>
#include <locale.h>
#include <libintl.h>

typedef struct {
  GtkWidget *treeview;
  GtkWidget *input_dlg_is_birthday;
  GtkWidget *input_dlg_date;
  GtkWidget *input_dlg_time;
  GtkWidget *input_dlg_text;
  GtkWidget *input_dlg_cycle;
} Data;

gchar exec_dir[1000];

void on_window1_destroy(GtkWidget *window, Data *data) {
  //list_save();
  gtk_main_quit();
}

void treeview1_remove_row(GtkTreeRowReference *ref, GtkTreeModel *model) {
  GtkTreeIter iter;
  GtkTreePath *path;

  path = gtk_tree_row_reference_get_path(ref);
  gtk_tree_model_get_iter(model, &iter, path);

  //int *i;
  //i = gtk_tree_path_get_indices(path);
  //list_delete_entry(i[0]);
  gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
}

void on_button_remove1_clicked(GtkWidget *button, Data *data) {
  GtkTreeSelection *selection;
  GtkTreeRowReference *ref;
  GtkTreeModel *model;
  GList *rows, *ptr, *references = NULL;

  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
  model = gtk_tree_view_get_model(GTK_TREE_VIEW(data->treeview));
  rows = gtk_tree_selection_get_selected_rows(selection, &model);

  ptr = rows;
  while (ptr != NULL) {
    ref = gtk_tree_row_reference_new(model, (GtkTreePath*)ptr->data);
    references = g_list_prepend(references, gtk_tree_row_reference_copy(ref));
    gtk_tree_row_reference_free(ref);
    ptr = ptr->next;
  }

  g_list_foreach(references, (GFunc)treeview1_remove_row, model);

  g_list_foreach(references, (GFunc)gtk_tree_row_reference_free, NULL);
  g_list_foreach(rows, (GFunc)gtk_tree_path_free, NULL);
  g_list_free(references);
  g_list_free(rows);
}

void on_dialog_button_ok1_clicked(GtkWidget *button, Data *data) {
  const gchar *date, *time, *text;
  gchar type_str[256];
  gint rcycle, i = 1;

  date = gtk_entry_get_text(GTK_ENTRY(data->input_dlg_date));
  time = gtk_entry_get_text(GTK_ENTRY(data->input_dlg_time));
  text = gtk_entry_get_text(GTK_ENTRY(data->input_dlg_text));
  rcycle = (gint)gtk_spin_button_get_value(GTK_SPIN_BUTTON(data->input_dlg_cycle));

  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(data->input_dlg_is_birthday)) == TRUE) {
    strcpy(type_str, gettext("Birthday"));
  } else if (rcycle > 0) {
    strcpy(type_str, gettext("Repeating"));
  } else {
    strcpy(type_str, gettext("Single"));
  }

  if (g_ascii_strcasecmp(date, "") == 0 || g_ascii_strcasecmp(date, gettext("DD.MM.YYYY")) == 0 ||
      g_ascii_strcasecmp(time, "") == 0 || g_ascii_strcasecmp(text, "") == 0) {
    g_warning(gettext("Cannot add event, not enough data given."));
    return;
  }

  /*st_gtk_list_item temp_item;
  strcpy(temp_item.type, type_str);
  strcpy(temp_item.date, date);
  strcpy(temp_item.time, time);
  strcpy(temp_item.text, text);
  temp_item.cycle = rcycle;
  i = set_gtk_list_item(&temp_item);*/
  if (i > -1) {
    GtkTreeIter iter;
    GtkTreeModel *model;

    model = gtk_tree_view_get_model(GTK_TREE_VIEW(data->treeview));
    //gtk_list_store_insert(GTK_LIST_STORE(model), &iter, i);
    gtk_list_store_append(GTK_LIST_STORE(model), &iter);
    gtk_list_store_set(GTK_LIST_STORE(model), &iter,
      0, type_str, 1, date, 2, time, 3, text, 4, rcycle, -1
    );
  } else {
    g_warning(gettext("Cannot add event, error while adding data to list."));
  }
}

void on_button_add1_clicked(GtkWidget *button, Data *data) {
  GtkBuilder *builder;
  GtkWidget *dialog;

  builder = gtk_builder_new();

  gchar dialog_ui_dir[1000];
  strncpy(dialog_ui_dir, exec_dir, 1000);
  strcat(dialog_ui_dir, "dialog.ui");

  gtk_builder_add_from_file(builder, dialog_ui_dir, NULL);

  dialog = GTK_WIDGET(gtk_builder_get_object(builder, "dialog1"));

  data->input_dlg_is_birthday = GTK_WIDGET(gtk_builder_get_object(builder, "checkbutton_is_birthday1"));
  data->input_dlg_date = GTK_WIDGET(gtk_builder_get_object(builder, "entry_date1"));
  data->input_dlg_time = GTK_WIDGET(gtk_builder_get_object(builder, "entry_time1"));
  data->input_dlg_text = GTK_WIDGET(gtk_builder_get_object(builder, "entry_text1"));
  data->input_dlg_cycle = GTK_WIDGET(gtk_builder_get_object(builder, "spinbutton_cycle1"));

  gtk_builder_connect_signals(builder, data);
  g_object_unref(G_OBJECT(builder));

  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}

void init_exec_dir(void) {
  char exec_dir_parts[10][100];

  readlink("/proc/self/exe", exec_dir, sizeof(exec_dir));

  char *ptr;
  ptr = strtok(exec_dir, "/");
  int i = 0, j = 0;
  while (ptr != NULL) {
    strncpy(exec_dir_parts[i], ptr, sizeof(exec_dir_parts[i]));
    ptr = strtok(NULL, "/");
    i++;
  }
  i--;
  strncpy(exec_dir, "/", sizeof(exec_dir));
  for (j = 0; j < i; j++) {
    strcat(exec_dir, exec_dir_parts[j]);
    strcat(exec_dir, "/");
  }
}

int main (int argc, char *argv[]) {
  GtkBuilder *builder;
  GtkWidget *window;
  Data data;

  init_exec_dir();

  setlocale(LC_ALL, "");
  gchar locale_dir[1000];
  strncpy(locale_dir, exec_dir, 1000);
  strcat(locale_dir, "locale");
  bindtextdomain("geventlist", locale_dir);
  textdomain("geventlist");

  //list_get_file();
  //list_load();
  //list_sort();

  gtk_init(&argc, &argv);
  builder = gtk_builder_new();

  gchar window_ui_dir[1000];
  strncpy(window_ui_dir, exec_dir, 1000);
  strcat(window_ui_dir, "window.ui");
  gtk_builder_add_from_file(builder, window_ui_dir, NULL);

  window = GTK_WIDGET(gtk_builder_get_object(builder, "window1"));
  data.treeview = GTK_WIDGET(gtk_builder_get_object(builder, "treeview1"));

  GtkTreeSelection *selection;
  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(data.treeview));
  gtk_tree_selection_set_mode(selection, GTK_SELECTION_MULTIPLE);

  /*model = gtk_tree_view_get_model(GTK_TREE_VIEW(data->treeview));
  guint i = 0;
  while (i < list_length) {
    st_gtk_list_item temp = get_gtk_list_item(i);

    gtk_list_store_append(GTK_LIST_STORE(model), &iter);
    gtk_list_store_set(GTK_LIST_STORE(model), &iter,
      0, temp.type, 1, temp.date, 2, temp.time,
      3, temp.text, 4, temp.cycle, -1
    );
    i++;
  }*/

  gtk_builder_connect_signals(builder, &data);
  g_object_unref(G_OBJECT(builder));

  gtk_widget_show_all(window);
  gtk_main();

  return 0;
}