#include <gtk/gtk.h>

typedef enum {
    EMPTY,
    PLAYER_X,
    PLAYER_O
} CellState;

CellState board[3][3];
int playerXWins = 0;
int playerOWins = 0;
bool playerXTurn = true;
GtkWidget *labels[3][3];

static void button_click(GtkButton *button, gpointer user_data) {
    if (gtk_button_get_label(button)[0] != ' ' || !playerXTurn)
        return;

    gtk_button_set_label(button, "X");
    gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

    int row = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "row"));
    int col = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "col"));
    board[row][col] = PLAYER_X;

    if (check_winner(PLAYER_X)) {
        g_print("Jogador X venceu!\n");
        playerXWins++;
        update_score();
        reset_board();
        return;
    } else if (is_board_full()) {
        g_print("Empate!\n");
        reset_board();
        return;
    }

    playerXTurn = false;
}

static void computer_play() {
    if (is_board_full())
        return;

    int row, col;
    do {
        row = rand() % 3;
        col = rand() % 3;
    } while (board[row][col] != EMPTY);

    board[row][col] = PLAYER_O;
    char label[2];
    snprintf(label, sizeof(label), "O");
    gtk_button_set_label(GTK_BUTTON(labels[row][col]), label);
    gtk_widget_set_sensitive(labels[row][col], FALSE);

    if (check_winner(PLAYER_O)) {
        g_print("Jogador O venceu!\n");
        playerOWins++;
        update_score();
        reset_board();
        return;
    } else if (is_board_full()) {
        g_print("Empate!\n");
        reset_board();
        return;
    }

    playerXTurn = true;
}

static void update_score() {
    char score_text[50];
    snprintf(score_text, sizeof(score_text), "Placar - Jogador X: %d, Jogador O: %d", playerXWins, playerOWins);
    gtk_label_set_text(GTK_LABEL(labels[0][3]), score_text);
}

static gboolean check_winner(CellState player) {
    for (int i = 0; i < 3; i++) {
        if (board[i][0] == player && board[i][1] == player && board[i][2] == player)
            return TRUE;
        if (board[0][i] == player && board[1][i] == player && board[2][i] == player)
            return TRUE;
    }

    if (board[0][0] == player && board[1][1] == player && board[2][2] == player)
        return TRUE;
    if (board[0][2] == player && board[1][1] == player && board[2][0] == player)
        return TRUE;

    return FALSE;
}

static gboolean is_board_full() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == EMPTY)
                return FALSE;
        }
    }
    return TRUE;
}

static void reset_board() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            board[i][j] = EMPTY;
            gtk_button_set_label(GTK_BUTTON(labels[i][j]), " ");
            gtk_widget_set_sensitive(labels[i][j], TRUE);
        }
    }

    if (!playerXTurn)
        computer_play();
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *button;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Jogo da Velha");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 300);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            button = gtk_button_new_with_label(" ");
            g_object_set_data(G_OBJECT(button), "row", GINT_TO_POINTER(i));
            g_object_set_data(G_OBJECT(button), "col", GINT_TO_POINTER(j));
            g_signal_connect(button, "clicked", G_CALLBACK(button_click), NULL);
            labels[i][j] = button;
            gtk_grid_attach(GTK_GRID(grid), button, j, i, 1, 1);
        }
    }

    GtkWidget *score_label = gtk_label_new("");
    gtk_grid_attach(GTK_GRID(grid), score_label, 0, 3, 3, 1);
    labels[0][3] = score_label;

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}
