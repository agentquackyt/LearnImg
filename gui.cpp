// Terminal prompt-based GUI for layered pattern generation using ncurses
#include <ncurses.h>
#include <dirent.h>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include "pgm.h"
#include "pattern.h"

struct Layer {
    std::string name;
    Pattern pattern;
    bool negated = false;
    char op = ' '; // ' ' for first, '&' = AND, '|' = OR, '^' = XOR
    Layer(const std::string &n, const Pattern &p) : name(n), pattern(p) {}
};

void drawLayers(WINDOW *win, const std::vector<Layer> &layers, int highlight)
{
    werase(win);
    box(win, 0, 0);
    mvwprintw(win, 0, 2, " Layers (apply sequentially) ");
    for (size_t i = 0; i < layers.size(); ++i) {
        if ((int)i == highlight) wattron(win, A_REVERSE);
        std::string line = std::to_string(i) + ": "+ layers[i].name + (layers[i].negated?" [NOT]":"");
        if (i>0) {
            line += " (";
            line += layers[i].op;
            line += ")";
        }
        mvwprintw(win, i+1, 2, "%s", line.c_str());
        if ((int)i == highlight) wattroff(win, A_REVERSE);
    }
    wrefresh(win);
}

Pattern combineLayers(const std::vector<Layer> &layers, int width, int height)
{
    if (layers.empty()) return Pattern(width, height);
    Pattern acc = layers[0].pattern; // copy
    if (layers[0].negated) acc = acc.operator!();
    for (size_t i = 1; i < layers.size(); ++i) {
        Pattern cur = layers[i].pattern;
        if (layers[i].negated) cur = cur.operator!();
        switch (layers[i].op) {
            case '&': acc = acc && cur; break;
            case '|': acc = acc || cur; break;
            case '^': acc = acc ^ cur; break;
            default: acc = cur; break;
        }
    }
    return acc;
}

void promptCentered(int y, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    char buf[256];
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    int x = (COLS - strlen(buf)) / 2;
    mvprintw(y, x, "%s", buf);
    clrtoeol();
}

int main()
{
    initscr(); cbreak(); noecho(); keypad(stdscr, TRUE); curs_set(0);
    int width = 128, height = 128;

    std::vector<Layer> layers;
    int highlight = 0;

    int win_h = LINES - 6;
    int win_w = COLS - 4;
    WINDOW *layerwin = newwin(win_h, win_w, 1, 2);

    mvprintw(LINES-4, 2, "Commands: [A] Add  [E] Edit  [D] Delete  [O] SetOp  [N] ToggleNOT  [U/J] MoveUp/Down");
    mvprintw(LINES-3, 2, "[P] Preview  [X] Export  [Q] Quit");
    refresh();

    bool running = true;
    while (running) {
        drawLayers(layerwin, layers, highlight);
        int ch = getch();
        switch (ch) {
            case 'q': case 'Q': running = false; break;
            case KEY_UP: highlight = (highlight>0)?highlight-1:0; break;
            case KEY_DOWN: highlight = (highlight+1<(int)layers.size())?highlight+1:highlight; break;

            case 'a': case 'A': {
                echo();
                promptCentered(LINES-7, "Add layer - name: ");
                char namebuf[128];
                mvgetnstr(LINES-6, (COLS-20)/2 + 13, namebuf, 120);
                promptCentered(LINES-7, "Type [c]ircle [t]riangle [b]checker [l]oad [m]aze: ");
                int type = getch();
                noecho();
                Pattern p(width, height);
                std::string name = namebuf;
                if (type == 'c') {
                    echo(); promptCentered(LINES-7, "Radius: "); int r; mvscanw(LINES-6, (COLS-20)/2 + 8, "%d", &r); noecho();
                    p = generateCirclePattern(width, height, r);
                    name += " (circle)";
                } else if (type == 't') {
                    p = generateTrianglePattern(width, height);
                    name += " (triangle)";
                } else if (type == 'b') {
                    echo(); promptCentered(LINES-7, "Square size: "); int s; mvscanw(LINES-6, (COLS-20)/2 + 14, "%d", &s); noecho();
                    p = generateCheckerboardPattern(width, height, s);
                    name += " (checker)";
                } else if (type == 'l') {
                    echo(); promptCentered(LINES-7, "Filename in ./patterns/ (e.g. gui_generated.pgm): "); char fnb[128]; mvgetnstr(LINES-6, (COLS-60)/2 + 34, fnb, 120); noecho();
                    p = loadPatternFromPgm(fnb);
                    if (p.width != width || p.height != height) {
                        // simple mismatch handling: reject
                        promptCentered(LINES-7, "Loaded size %dx%d != current %dx%d (press any key)", p.width, p.height, width, height);
                        getch();
                        p = Pattern(width, height);
                        name += " (load:fail)";
                    } else name += std::string(" (loaded: ") + fnb + ")";
                } else if (type == 'm') {
                    // Maze: carve on empty base
                    Pattern base(width, height);
                    Pattern visited(width, height);
                    p = labyrinthPatternGenerator(base, visited, 0, 0);
                    name += " (maze)";
                } else {
                    promptCentered(LINES-7, "Unknown type (press any key)"); getch();
                    break;
                }
                layers.emplace_back(name, p);
                highlight = layers.size()-1;
                break;
            }

            case 'd': case 'D': {
                if (!layers.empty()) {
                    layers.erase(layers.begin() + highlight);
                    highlight = std::max(0, highlight-1);
                }
                break;
            }

            case 'e': case 'E': {
                if (layers.empty()) break;
                echo(); promptCentered(LINES-7, "Rename: "); char newn[128]; mvgetnstr(LINES-6, (COLS-20)/2 + 9, newn, 120); noecho();
                layers[highlight].name = newn;
                break;
            }

            case 'o': case 'O': {
                if (layers.size() < 2 || highlight==0) break;
                promptCentered(LINES-7, "Operator for this layer: [&] AND  [|] OR  [^] XOR");
                int op = getch();
                if (op == '&' || op == '|' || op == '^') layers[highlight].op = (char)op;
                break;
            }

            case 'n': case 'N': {
                if (!layers.empty()) layers[highlight].negated = !layers[highlight].negated;
                break;
            }

            case 'u': case 'U': {
                if (highlight>0) {
                    std::swap(layers[highlight], layers[highlight-1]);
                    highlight--;
                }
                break;
            }
            case 'j': case 'J': {
                if (highlight+1 < (int)layers.size()) {
                    std::swap(layers[highlight], layers[highlight+1]);
                    highlight++;
                }
                break;
            }

            case 'p': case 'P': {
                if (layers.empty()) {
                    promptCentered(LINES-7, "No layers to preview (press any key)"); getch(); break;
                }
                Pattern combined = combineLayers(layers, width, height);
                savePatternAsPgm(combined, "gui_preview.pgm");
                promptCentered(LINES-7, "Preview saved to ./patterns/gui_preview.pgm (press any key)"); getch();
                break;
            }

            case 'x': case 'X': {
                // Export options
                promptCentered(LINES-7, "Export as [5] P5 (PGM) or [6] P6 (PPM): "); int t = getch();
                if (t == '5') {
                    echo(); promptCentered(LINES-7, "Output filename (in ./patterns/): "); char ofn[128]; mvgetnstr(LINES-6, (COLS-40)/2 + 28, ofn, 120); noecho();
                    Pattern combined = combineLayers(layers, width, height);
                    savePatternAsPgm(combined, ofn);
                    promptCentered(LINES-7, "Saved ./patterns/%s (press any key)", ofn); getch();
                } else if (t == '6') {
                    promptCentered(LINES-7, "P6 mode: [1] 3-layer  [2] Single combined->channel");
                    int pmode = getch();
                    if (pmode == '1') {
                        if (layers.size() < 3) {
                            promptCentered(LINES-7, "Need at least 3 layers to export P6 (press any key)"); getch(); break;
                        }
                        echo(); promptCentered(LINES-7, "Index for R layer: "); int ri; mvscanw(LINES-6, (COLS-40)/2 + 16, "%d", &ri);
                        promptCentered(LINES-7, "Index for G layer: "); int gi; mvscanw(LINES-6, (COLS-40)/2 + 16, "%d", &gi);
                        promptCentered(LINES-7, "Index for B layer: "); int bi; mvscanw(LINES-6, (COLS-40)/2 + 16, "%d", &bi);
                        promptCentered(LINES-7, "Base value (1-255): "); int bv; mvscanw(LINES-6, (COLS-40)/2 + 16, "%d", &bv);
                        promptCentered(LINES-7, "Output filename (in ./patterns/): "); char ofn[128]; mvgetnstr(LINES-6, (COLS-40)/2 + 28, ofn, 120);
                        noecho();
                        if (ri < 0 || gi < 0 || bi < 0 || ri >= (int)layers.size() || gi >= (int)layers.size() || bi >= (int)layers.size()) {
                            promptCentered(LINES-7, "Invalid indices (press any key)"); getch(); break;
                        }
                        patternMixer(layers[ri].pattern, layers[gi].pattern, layers[bi].pattern, bv, ofn);
                        promptCentered(LINES-7, "Saved ./patterns/%s (press any key)", ofn); getch();
                    } else if (pmode == '2') {
                        // Single channel export: combined pattern goes into one channel
                        echo(); promptCentered(LINES-7, "Channel to fill: [r] [g] [b]: "); int chsel = getch();
                        promptCentered(LINES-7, "Base value (1-255): "); int bv; mvscanw(LINES-6, (COLS-40)/2 + 16, "%d", &bv);
                        promptCentered(LINES-7, "Output filename (in ./patterns/): "); char ofn[128]; mvgetnstr(LINES-6, (COLS-40)/2 + 28, ofn, 120); noecho();
                        Pattern combined = combineLayers(layers, width, height);
                        Pattern pr(width, height), pg(width, height), pb(width, height);
                        for (int i = 0; i < width*height; ++i) {
                            bool v = combined.data[i];
                            pr.data[i] = (chsel == 'r' || chsel == 'R') ? v : false;
                            pg.data[i] = (chsel == 'g' || chsel == 'G') ? v : false;
                            pb.data[i] = (chsel == 'b' || chsel == 'B') ? v : false;
                        }
                        patternMixer(pr, pg, pb, bv, ofn);
                        promptCentered(LINES-7, "Saved ./patterns/%s (press any key)", ofn); getch();
                    }
                }
                break;
            }

            case 'w': case 'W': {
                // change global dimensions
                echo(); promptCentered(LINES-7, "Width: "); mvscanw(LINES-6, (COLS-40)/2 + 8, "%d", &width);
                promptCentered(LINES-7, "Height: "); mvscanw(LINES-6, (COLS-40)/2 + 9, "%d", &height);
                noecho();
                promptCentered(LINES-7, "Dimensions set to %dx%d (future layers only) (press any key)", width, height); getch();
                break;
            }
        }
    }

    delwin(layerwin);
    endwin();
    return 0;
}
