#include <gtk/gtk.h>
#include <string.h>
#include "eventlist.h"
#include "phrases.h"

extern st_phrases phrases_data;

extern st_list_entry list[MAX_LIST_ENTRIES];
extern int list_length;

enum {
  TYPE,
  DATE,
  TIME,
  TEXT,
  CYCLE,
  COLUMNS
};

static void destroy(GtkWidget*, gpointer);

static void setup_tree_view(GtkWidget*);

static void add_event(GtkButton*, GtkTreeView*);

static void remove_events(GtkButton*, GtkTreeView*);

int main (int argc, char *argv[]) {
  phrases_set_lang("de-DE");
  phrases_load();

  list_get_file();
  list_load();

  GtkWidget *window, *vbox, *treeview, *scrolled_win;
  GtkWidget *hbox_buttons, *button_add, *button_remove;
  GtkListStore *store;
  GtkTreeIter iter;
  GtkTreeSelection *selection;

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

  guint i = 0;
  while (i < list_length) {
    st_gtk_list_item temp = get_gtk_list_item(i);

    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter,
      TYPE, temp.type, DATE, temp.date, TIME, temp.time,
      TEXT, temp.text, CYCLE, temp.cycle,
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
  list_save();
  gtk_main_quit();
}

static void setup_tree_view(GtkWidget *treeview) {
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;

  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes(
    phrases_data.column_type, renderer, "text", TYPE, NULL
  );
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes(
    phrases_data.column_date, renderer, "text", DATE, NULL
  );
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes(
    phrases_data.column_type, renderer, "text", TIME, NULL
  );
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes(
    phrases_data.column_text, renderer, "text", TEXT, NULL
  );
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes(
    phrases_data.column_cycle, renderer, "text", CYCLE, NULL
  );
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
}

static void add_event(GtkButton *add, GtkTreeView *treeview) {
  GtkWidget *dialog, *table;
  GtkWidget *entry_date, *entry_time, *entry_text, *check_birthday, *spin_rcycle;
  GtkTreeIter iter;
  GtkTreeModel *model;

  const gchar *date, *time, *text;
  gchar phrase_option_title[PHRASES_CHARS_LENGTH];
  gint i, rcycle;

  dialog = gtk_dialog_new_with_buttons(
    phrases_data.title_add_event, NULL, GTK_DIALOG_MODAL,
    GTK_STOCK_ADD, GTK_RESPONSE_OK,
    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);

  check_birthday = gtk_check_button_new_with_mnemonic(phrases_data.option_is_birthday);
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

  strcpy(phrase_option_title, phrases_data.column_date);
  strcat(phrase_option_title, ":");
  gtk_table_attach(GTK_TABLE(table), gtk_label_new(phrase_option_title), 0, 1, 1, 2,
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
  gtk_table_attach(GTK_TABLE(table), entry_date, 1, 2, 1, 2, GTK_EXPAND | GTK_FILL,
                   GTK_SHRINK | GTK_FILL, 0, 0);
  gtk_entry_set_text(GTK_ENTRY(entry_date), phrases_data.option_date_text);

  strcpy(phrase_option_title, phrases_data.column_time);
  strcat(phrase_option_title, ":");
  gtk_table_attach(GTK_TABLE(table), gtk_label_new(phrase_option_title), 0, 1, 2, 3,
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
  gtk_table_attach(GTK_TABLE(table), entry_time, 1, 2, 2, 3, GTK_EXPAND | GTK_FILL,
                   GTK_SHRINK | GTK_FILL, 0, 0);
  gtk_entry_set_text(GTK_ENTRY(entry_time), "00:00");

  strcpy(phrase_option_title, phrases_data.column_text);
  strcat(phrase_option_title, ":");
  gtk_table_attach(GTK_TABLE(table), gtk_label_new(phrase_option_title), 0, 1, 3, 4,
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
  gtk_table_attach(GTK_TABLE(table), entry_text, 1, 2, 3, 4, GTK_EXPAND | GTK_FILL,
                   GTK_SHRINK | GTK_FILL, 0, 0);

  strcpy(phrase_option_title, phrases_data.column_cycle);
  strcat(phrase_option_title, ":");
  gtk_table_attach(GTK_TABLE(table), gtk_label_new(phrase_option_title), 0, 1, 4, 5,
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

    gchar type_str[256];
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_birthday)) == TRUE) {
      strcpy(type_str, phrases_data.option_type_birthday);
    } else if (rcycle > 0) {
      strcpy(type_str, phrases_data.option_type_repeating);
    } else {
      strcpy(type_str, phrases_data.option_type_single);
    }

    if (g_ascii_strcasecmp(date, "") == 0 ||
        g_ascii_strcasecmp(date, phrases_data.option_date_text) == 0 ||
        g_ascii_strcasecmp(time, "") == 0 || g_ascii_strcasecmp(text, "") == 0) {
      g_warning("Cannot add item to list, not enough data given!");
      gtk_widget_destroy(dialog);
      return;
    }

    st_gtk_list_item temp_item;
    strcpy(temp_item.type, type_str);
    strcpy(temp_item.date, date);
    strcpy(temp_item.time, time);
    strcpy(temp_item.text, text);
    temp_item.cycle = rcycle;

    i = set_gtk_list_item(&temp_item);
    if (i > -1) {
      model = gtk_tree_view_get_model(treeview);
      gtk_list_store_insert(GTK_LIST_STORE(model), &iter, i);

      gtk_list_store_set(GTK_LIST_STORE(model), &iter,
        TYPE, temp_item.type, DATE, temp_item.date, TIME, temp_item.time,
        TEXT, temp_item.text, CYCLE, temp_item.cycle, -1
      );
    } else {
      g_warning("Cannot add item to list, no new position available!");
    }
  }

  gtk_widget_destroy(dialog);
}

static void remove_row(GtkTreeRowReference *ref, GtkTreeModel *model) {
  GtkTreeIter iter;
  GtkTreePath *path;
  int *i;

  path = gtk_tree_row_reference_get_path(ref);
  gtk_tree_model_get_iter(model, &iter, path);

  i = gtk_tree_path_get_indices(path);
  list_delete_entry(i[0]);

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
    references = g_list_prepend(references, gtk_tree_row_reference_copy(ref));
    gtk_tree_row_reference_free(ref);
    ptr = ptr->next;
  }

  g_list_foreach(references, (GFunc)remove_row, model);

  g_list_foreach(references, (GFunc)gtk_tree_row_reference_free, NULL);
  g_list_foreach(rows, (GFunc)gtk_tree_path_free, NULL);
  g_list_free(references);
  g_list_free(rows);
}