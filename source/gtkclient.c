#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>

void breakstr(const char * src, char * dest, int maxlinelen, unsigned long n) {
    strncpy(dest, src, n);
    for (unsigned long i = 0, last_space = -1; i < n && dest[i] != '\0'; i++) {
        if (dest[i] == ' ')
            last_space = i;
        if (i%maxlinelen == 0 && i) {
            if (last_space == -1)
                while (dest[i++] != ' ')
                    if (dest[i] == '\0')
                        return;
            dest[last_space] = '\n';
            last_space = -1;
        }
    }
}

char cards[25][200];

static void
activate (GtkApplication *app,
          gpointer        user_data)
{
  GtkWidget *window;
  GtkWidget *box;
  GtkWidget *grid;
  GtkWidget *button;
  GtkWidget *label;

  /* create a new window, and set its title */
  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Window");

  box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_set_halign (box, GTK_ALIGN_CENTER);
  gtk_widget_set_valign (box, GTK_ALIGN_CENTER);

  gtk_window_set_child (GTK_WINDOW (window), box);

  /* Here we construct the container that is going pack our buttons */
  grid = gtk_grid_new ();

  gtk_box_append(GTK_BOX(box), grid);

  /* Pack the container in the window */

  guint space = 2;

  gtk_grid_set_row_spacing( grid, space );
  gtk_grid_set_column_spacing( grid, space );

  for (int i = 0; i < 5; i++)
      for (int j = 0; j < 5; j++) {
          char buf[200];
          breakstr(cards[i*5+j], buf, 20, 200);
          button = gtk_toggle_button_new_with_label (buf);
          g_signal_connect (button, "clicked", NULL, NULL);
          gtk_grid_attach (GTK_GRID (grid), button, i, j, 1, 1);
      }

  /* Place the second button in the grid cell (2, 0), and make it fill
   * just 1 cell horizontally and vertically (ie no spanning)
   */
  gtk_grid_attach (GTK_GRID (grid), button, 2, 0, 1, 1);

  button = gtk_button_new_with_label ("Quit");
  g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_window_destroy), window);

  /* Place the Quit button in the grid cell (0, 1), and make it
   * span 2 columns.
   */
  gtk_grid_attach (GTK_GRID (grid), button, 0, 6, 5, 1);

  gtk_window_fullscreen (window);
  gtk_widget_show (window);

}

int
main (int    argc,
      char **argv)
{
  GtkApplication *app;
  int status;
  char buf[200];

  FILE * f = fopen("./cards_hc.txt", "r");
  if (f == NULL) {
      puts(getcwd(buf, sizeof(buf)));
      puts("Failed to open text file");
      return 1;
  }
  for (int i = 0; i < 25; i++) {
        fgets(cards[i], 200, f);
  }

  app = gtk_application_new ("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}
