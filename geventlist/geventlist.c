#include <gtk/gtk.h>
#include <string.h>

enum {
  TYPE,
  DATE,
  TIME,
  TEXT,
  CYCLE,
  COLUMNS
};

typedef struct {
  gint type;
  gchar *date;
  gchar *time;
  gchar *text;
  gint cycle;
} EventListItem;

const EventListItem list[] = {
  { 2, "6.7.2014", "00:00", "Birthday 1", 0 },
  { 0, "8.11.2014", "00:00", "Single 1", 0 },
  { 1, "12.12.2014", "00:00", "Repeating 1", 7 },
  { -1, "", "", "", 0 }
};

static void destroy(GtkWidget*, gpointer);

static void setup_tree_view(GtkWidget*);

static void add_event(GtkButton*, GtkTreeView*);

static void remove_events(GtkButton*, GtkTreeView*);

int main (int argc, char *argv[]) {

  GtkWidget *window, *vbox, *treeview, *scrolled_win;
  GtkWidget *hbox_buttons, *button_add, *button_remove;
  GtkListStore *store;
  GtkTreeIter iter;
  GtkTreeSelection *selection;
  guint i = 0;

  gtk_init(&argc, &argv);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), "gEventlist");
  gtk_container_set_border_width(GTK_CONTAINER(window), 10);
  gtk_widget_set_size_request(window, 480, 360);

  g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(destroy), NULL);

  // teeview / scrolled_win
  treeview = gtk_tree_view_new();
  setup_tree_view(treeview);

  store = gtk_list_store_new(COLUMNS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT);

  while (list[i].type != -1) {
    gtk_list_store_append(store, &iter);

    gchar type_str[256];
    switch (list[i].type) {
      case 0:
        strcpy(type_str, "Single");
        break;
      case 1:
        strcpy(type_str, "Repeating");
        break;
      case 2:
        strcpy(type_str, "Birthday");
        break;
    }

    gtk_list_store_set(store, &iter,
      TYPE, type_str, DATE, list[i].date, TIME, list[i].time, TEXT, list[i].text, CYCLE, list[i].cycle,
      -1
    );
    i++;
  }

  gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store));
  g_object_unref(store);

  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
  gtk_tree_selection_set_mode(selection, GTK_SELECTION_MULTIPLE);

  scrolled_win = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW (scrolled_win),
                                 GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_container_add(GTK_CONTAINER(scrolled_win), treeview);

  // button bar
  button_add = gtk_button_new_from_stock(GTK_STOCK_ADD);
  button_remove = gtk_button_new_from_stock(GTK_STOCK_REMOVE);

  hbox_buttons = gtk_hbox_new(TRUE, 5);
  gtk_box_pack_start(GTK_BOX(hbox_buttons), button_add, FALSE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(hbox_buttons), button_remove, FALSE, TRUE, 0);

  // button events
  g_signal_connect(G_OBJECT(button_add), "clicked",
                   G_CALLBACK(add_event), (gpointer)treeview);
  g_signal_connect(G_OBJECT(button_remove), "clicked",
                   G_CALLBACK(remove_events), (gpointer)treeview);

  // vbox
  vbox = gtk_vbox_new(FALSE, 5);
  gtk_box_pack_start (GTK_BOX(vbox), scrolled_win, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX(vbox), hbox_buttons, FALSE, TRUE, 0);

  gtk_container_add(GTK_CONTAINER(window), vbox);

  gtk_widget_show_all(window);

  gtk_main ();
  return 0;
}

static void destroy (GtkWidget *window, gpointer data) {
  gtk_main_quit();
}

static void setup_tree_view(GtkWidget *treeview) {
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;

  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes("Type", renderer, "text", TYPE, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes("Date", renderer, "text", DATE, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes("Time", renderer, "text", TIME, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes("Text", renderer, "text", TEXT, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes("Cycle", renderer, "text", CYCLE, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
}

static void add_event(GtkButton *add, GtkTreeView *treeview) {
  GtkWidget *dialog, *table;
  GtkWidget *entry_date, *entry_time, *entry_text, *check_birthday, *spin_rcycle;
  GtkTreeIter iter;
  GtkTreeModel *model;

  const gchar *date, *time, *text;
  gchar *selected_date, *selected_time;
  gint type, rcycle;

  dialog = gtk_dialog_new_with_buttons(
    "Add an event", NULL, GTK_DIALOG_MODAL,
    GTK_STOCK_ADD, GTK_RESPONSE_OK,
    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);

  check_birthday = gtk_check_button_new_with_mnemonic("is birthday");
  entry_date = gtk_entry_new();
  entry_time = gtk_entry_new();
  entry_text = gtk_entry_new();
  spin_rcycle = gtk_spin_button_new_with_range(0, 365, 1);

  table = gtk_table_new(5, 2, FALSE);
  gtk_table_set_row_spacings(GTK_TABLE(table), 5);
  gtk_table_set_col_spacings(GTK_TABLE(table), 5);
  gtk_container_set_border_width(GTK_CONTAINER(table), 5);

  gtk_table_attach(GTK_TABLE(table), check_birthday, 1, 2, 0, 1,
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);

  gtk_table_attach(GTK_TABLE(table), gtk_label_new("Date:"), 0, 1, 1, 2,
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
  gtk_table_attach(GTK_TABLE(table), entry_date, 1, 2, 1, 2, GTK_EXPAND | GTK_FILL,
                   GTK_SHRINK | GTK_FILL, 0, 0);
  gtk_entry_set_text(GTK_ENTRY(entry_date), "DD.MM.YYYY");

  gtk_table_attach(GTK_TABLE(table), gtk_label_new("Time:"), 0, 1, 2, 3,
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
  gtk_table_attach(GTK_TABLE(table), entry_time, 1, 2, 2, 3, GTK_EXPAND | GTK_FILL,
                   GTK_SHRINK | GTK_FILL, 0, 0);
  gtk_entry_set_text(GTK_ENTRY(entry_time), "00:00");

  gtk_table_attach(GTK_TABLE(table), gtk_label_new("Text:"), 0, 1, 3, 4,
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
  gtk_table_attach(GTK_TABLE(table), entry_text, 1, 2, 3, 4, GTK_EXPAND | GTK_FILL,
                   GTK_SHRINK | GTK_FILL, 0, 0);

  gtk_table_attach(GTK_TABLE(table), gtk_label_new("Cylce:"), 0, 1, 4, 5,
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
  gtk_table_attach(GTK_TABLE(table), spin_rcycle, 1, 2, 4, 5, GTK_EXPAND | GTK_FILL,
                   GTK_SHRINK | GTK_FILL, 0, 0);

  gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(dialog)->vbox), table);

  gtk_widget_show_all(dialog);

  if (gtk_dialog_run(GTK_DIALOG (dialog)) == GTK_RESPONSE_OK) {
    date = gtk_entry_get_text(GTK_ENTRY(entry_date));
    time = gtk_entry_get_text(GTK_ENTRY(entry_time));
    text = gtk_entry_get_text(GTK_ENTRY(entry_text));
    rcycle = (gint)gtk_spin_button_get_value (GTK_SPIN_BUTTON(spin_rcycle));
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_birthday)) == TRUE) {
      type = 2;
    } else if (rcycle > 0) {
      type = 1;
    } else {
      type = 0;
    }

    if (g_ascii_strcasecmp(date, "") == 0 || g_ascii_strcasecmp(date, "DD.MM.YYYY") == 0 ||
        g_ascii_strcasecmp(time, "") == 0 || g_ascii_strcasecmp(text, "") == 0) {
      g_warning ("All of the fields were not correctly filled out!");
      gtk_widget_destroy (dialog);
      return;
    }

    model = gtk_tree_view_get_model(treeview);
    gtk_tree_model_get_iter_from_string(model, &iter, "0");

    int i = 0;
    do {
      gtk_tree_model_get(model, &iter, DATE, &selected_date, TIME, &selected_time, -1);
      i++;
      if (g_ascii_strcasecmp(selected_date, date) == 0) {
        // todo get correct insert position by seperate event list
        g_message("Found date!");
        g_free(selected_date);
        g_free(selected_time);
        break;
      }

      g_free(selected_date);
      g_free(selected_time);
    } while (gtk_tree_model_iter_next(model, &iter));

    gtk_list_store_insert(GTK_LIST_STORE(model), &iter, i);

    gchar type_str[256];
    switch (type) {
      case 0:
        strcpy(type_str, "Single");
        break;
      case 1:
        strcpy(type_str, "Repeating");
        break;
      case 2:
        strcpy(type_str, "Birthday");
        break;
    }
    gtk_list_store_set(GTK_LIST_STORE(model), &iter,
      TYPE, type_str, DATE, date, TIME, time, TEXT, text, CYCLE, rcycle, -1
    );
  }

  gtk_widget_destroy(dialog);
}

static void remove_row(GtkTreeRowReference *ref, GtkTreeModel *model) {
  GtkTreeIter iter;
  GtkTreePath *path;

  path = gtk_tree_row_reference_get_path (ref);
  gtk_tree_model_get_iter(model, &iter, path);
  gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
}

static void remove_events(GtkButton *remove, GtkTreeView *treeview) {
  GtkTreeSelection *selection;
  GtkTreeRowReference *ref;
  GtkTreeModel *model;
  GList *rows, *ptr, *references = NULL;

  selection = gtk_tree_view_get_selection(treeview);
  model = gtk_tree_view_get_model(treeview);
  rows = gtk_tree_selection_get_selected_rows(selection, &model);

  ptr = rows;
  while (ptr != NULL) {
    ref = gtk_tree_row_reference_new(model, (GtkTreePath*)ptr->data);
    references = g_list_prepend(references, gtk_tree_row_reference_copy (ref));
    gtk_tree_row_reference_free(ref);
    ptr = ptr->next;
  }

  g_list_foreach(references, (GFunc)remove_row, model);

  g_list_foreach(references, (GFunc)gtk_tree_row_reference_free, NULL);
  g_list_foreach(rows, (GFunc)gtk_tree_path_free, NULL);
  g_list_free(references);
  g_list_free(rows);
}