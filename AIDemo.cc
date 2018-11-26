#include "Player.hh"


/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME kt

// DISCLAIMER: The following Demo player is *not* meant to do anything
// sensible. It is provided just to illustrate how to use the API.
// Please use AINull.cc as a template for your player.

struct PLAYER_NAME : public Player {

  /**
   * Factory: returns a new instance of this class.
   * Do not modify this function.
   */
  static Player* factory () {
    return new PLAYER_NAME;
  }

  /**
   * Types and attributes for your player can be defined here.
   */
  int turn = 0;
  int dx[9] = {1, 1, 0, -1, -1, -1, 0, 1, 0};
  int dy[9] = {0, 1, 1, 1, 0, -1, -1, -1, 0};
  vector<int> t; //Quines ciutats atacar, en general.
  map<int, Pos> target; // Quin objectiu te cada soldat.
  vector< vector<cell> > map(rows, vector<int>(cols));
  
  void init() {
    for (int i = 0; i < rows; ++i) {
      for (int j = 0; j < cols; ++j) map[i][j] = cell(i, j);
    }
  }
  
  int nearest_water(Pos p) {
    vector< vector<cell> > m(rows, vector<int>(cols, -1));
  }
  
  int nearest_food(Pos p) {
    
  }

  void move_warriors() {
    if (round()% 4 != me()) return; // This line makes a lot of sense.
    vector<int> w = warriors(me());
    vector< vector<Pos> > c = cities();
    int n = w.size();
    if (turn%20 == 0) {
      t = random_permutation(8);
      for (int i = 0; i < 120; ++i) {
        int x = t[random(0, 2)];
        target[i] = c[x][random(0, c[x].size() - 1)];
      }
    }

    for (int i = 0; i < n; ++i) {
      int id = w[i];
      Unit u = unit(id);
      Pos p = u.pos;
      Pos y = target[id];
      if (can_move(id) and p.i < y.i) command(id, Right);
      if (can_move(id) and p.i > y.i) command(id, Left);
      if (can_move(id) and p.j < y.j) command(id, Bottom);
      if (can_move(id) and p.j < y.j) command(id, Top);
    }
    ++turn;
  }

  void move_cars() {
    vector<int> C = cars(me());
    for (int id : C) {
      if (can_move(id)) { // This also makes sense.
        command(id, Dir(random(0, 7)));
      }
   }
  }


  /**
   * Play method, invoked once per each round.
   */
  void play () {
    init();
    move_warriors();
    move_cars();
    ++turn;
  }
};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
