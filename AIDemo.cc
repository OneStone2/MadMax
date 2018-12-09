#include "Player.hh"


/**
* Write the name of your player and save this file
* with the same name and .cc extension.
*/
#define PLAYER_NAME Vox

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

	typedef vector<int> VI;
	typedef vector<VI> VVI;
	typedef pair<int, int> ii;
	int dx[9] = {1, 1, 0, -1, -1, -1, 0, 1, 0};
	int dy[9] = {0, 1, 1, 1, 0, -1, -1, -1, 0};
	map<int, int> kind;
	
	VVI board; //tauler
	VVI dist_w; //distancia a aigua
	VVI dist_c; //distancia a ciutat
	VVI dist_uc; //distancia a ciutat no conquerida
	VVI dist_a; //distancia per ser atropellat
	VVI dist_f; //distancia a fuel
	VVI dist_u; //distancia a una victima

	bool valid(int i, int j) {
		if (i < 0 or j < 0) return false;
		if (i >= rows() or j >= cols()) return false;
		return true;
	}
	bool passable_w(int i, int j) {
		if (!valid(i, j)) return false;
		if (board[i][j] == Desert) return true;
		if (board[i][j] == Road) return true;
		if (board[i][j] == City) return true;
		return false;
	}
	bool passable_c(int i, int j) {
		if (!valid(i, j)) return false;
		if (board[i][j] == Desert) return true;
		if (board[i][j] == Road) return true;
		return false;
	}
	
	void init() {
		board = VVI(rows(), VI(cols()));
		if (round()% 4 == me()) {
			dist_w = VVI(rows(), VI(cols(), -1000));
			dist_c = VVI(rows(), VI(cols(), -1000));
			dist_uc = VVI(rows(), VI(cols(), -1000));
			dist_a = VVI(rows(), VI(cols(), -1000));
		}
		dist_u = VVI(rows(), VI(cols(), -1000));
		dist_f = VVI(rows(), VI(cols(), -1000));
		queue<ii> qw, qc, quc;
		priority_queue<pair<int, ii> > qf, qa, qu; 
		for (int i = 0; i < rows(); ++i) {
			for (int j = 0; j < cols(); ++j) {
				Cell c = cell(i, j);
				board[i][j] = c.type;
				if (round()% 4 == me() and c.type == Water) {
					dist_w[i][j] = 0;
					qw.push({i, j});
				}
				else if (round()% 4 == me() and c.type == City) {
					dist_c[i][j] = 0;
					qc.push({i, j});
					if (c.owner != me()) {
						dist_uc[i][j] = 0;
						quc.push({i, j});
					}
				}
				else if (c.type == Station) {
					dist_f[i][j] = 0;
					qf.push({0, {i, j}});
				}
			}
		}
		for (int i = 0; round()% 4 == me() and i < 4; ++i) {
			if (i != me()) {
				vector<int> ca = cars(i);
				for (int j = 0; j < ca.size(); ++j) {
					Unit u = unit(ca[j]);
					int x = u.pos.i, y = u.pos.j;
					dist_a[x][y] = 0;
					qa.push({0, {x, y}});
				}
			}
		}
		for (int i = 0; i < 4; ++i) {
			if (i != me()) {
				vector<int> w = warriors(i);
				for (int j = 0; j < w.size(); ++j) {
					Unit u = unit(w[j]);
					int x = u.pos.i, y = u.pos.j;
					dist_u[x][y] = 0;
					qu.push({0, {x, y}});
				}
			}
		}
		while (round()% 4 == me() and qw.size() > 0) {
			int x0 = qw.front().first, y0 = qw.front().second;
			int w0 = dist_w[x0][y0];
			qw.pop();
			for (int i = 0; i < 8; ++i) {
				int x = x0 + dx[i], y = y0 + dy[i];
				if (passable_w(x, y) and dist_w[x][y] == -1000) {
					dist_w[x][y] = w0 - 1;
					qw.push({x, y});
				}
			}
		}
		while (round()% 4 == me() and quc.size() > 0) {
			int x0 = quc.front().first, y0 = quc.front().second;
			int w0 = dist_uc[x0][y0];
			quc.pop();
			for (int i = 0; i < 8; ++i) {
				int x = x0 + dx[i], y = y0 + dy[i];
				if (passable_w(x, y) and dist_uc[x][y] == -1000) {
					dist_uc[x][y] = w0 - 1;
					quc.push({x, y});
				}
			}
		}
		while (round()% 4 == me() and qc.size() > 0) {
			int x0 = qc.front().first, y0 = qc.front().second;
			int w0 = dist_c[x0][y0];
			qc.pop();
			for (int i = 0; i < 8; ++i) {
				int x = x0 + dx[i], y = y0 + dy[i];
				if (passable_w(x, y) and dist_c[x][y] == -1000) {
					dist_c[x][y] = w0 - 1;
					qc.push({x, y});
				}
			}
		}
		while (round()% 4 == me() and qa.size() > 0) {
			int x0 = qa.top().second.first, y0 = qa.top().second.second;
			int w0 = dist_a[x0][y0], w1 = qa.top().first;
			qa.pop();
			if (w0 == w1) {
				for (int i = 0; i < 8; ++i) {
					int x = x0 + dx[i], y = y0 + dy[i];
					if (passable_c(x, y)) {
						int w = dist_a[x][y];
						int w2 = w0;
						if (board[x0][y0] == Road) --w2;
						else w2 -= 4;
						if (w2 > w) {
							dist_a[x][y] = w2;
							qa.push({w2, {x, y}});
						}
					}
				}
			}
		}
		while (qf.size() > 0) {
			int x0 = qf.top().second.first, y0 = qf.top().second.second;
			int w0 = dist_f[x0][y0], w1 = qf.top().first;
			qf.pop();
			if (w0 == w1) {
				for (int i = 0; i < 8; ++i) {
					int x = x0 + dx[i], y = y0 + dy[i];
					if (passable_c(x, y)) {
						int w = dist_f[x][y];
						int w2 = w0;
						if (board[x0][y0] == Road) --w2;
						else w2 -= 4;
						if (w2 > w) {
							dist_f[x][y] = w2;
							qf.push({w2, {x, y}});
						}
					}
				}
			}
		}
		while (qu.size() > 0) {
			int x0 = qu.top().second.first, y0 = qu.top().second.second;
			int w0 = dist_u[x0][y0], w1 = qu.top().first;
			qu.pop();
			if (w0 == w1) {
				for (int i = 0; i < 8; ++i) {
					int x = x0 + dx[i], y = y0 + dy[i];
					if (passable_c(x, y)) {
						int w = dist_u[x][y];
						int w2 = w0;
						if (board[x][y] == Road) --w2;
						else w2 -= 4;
						if (w2 > w) {
							dist_u[x][y] = w2;
							qu.push({w2, {x, y}});
						}
					}
				}
			}
		}
	}

	void move_warriors() {
		if (round()% 4 != me()) return; // This line makes a lot of sense.

		VI w = warriors(me());
		int n = w.size();
		set<ii> occupied;
		for (int i = 0; i < n; ++i) {
			Unit u = unit(w[i]);
			int x0 = u.pos.i, y0 = u.pos.j;
			occupied.insert({x0, y0});
		}
		for (int i = 0; i < n; ++i) {
			Unit u = unit(w[i]);
			int x0 = u.pos.i, y0 = u.pos.j;
			int best = 8, value = -99999999;
			int x1 = x0, y1 = y0;
			for (int j = 0; j < 9; ++j) {
				int x = x0 + dx[j], y = y0 + dy[j];
				if (passable_w(x, y) and occupied.count({x, y}) == 0 and dist_a[x][y] < -4) {
					int score = 0;
					int z = cell(x, y).id;
					if (cell(x, y).type != City and z != -1) {
						Unit v = unit(z);
						if (v.type == Warrior) score += 1000000;
					}
					if (u.water < 20) score += dist_w[x][y];
					else if (u.food < 20) score =+ dist_c[x][y];
					else score += 4*dist_uc[x][y] - dist_a[x][y];
					if (score > value) {
						best = j;
						value = score;
						x1 = x, y1 = y;
					}
				}
			}
			occupied.insert({x1, y1});
			command(w[i], Dir(best));
		}
	}

	void move_cars() {
		map<ii, int> occupied;
		for (int i = 0; i < 4; ++i) {
			VI ca = cars(i);
			for (int j = 0; j < ca.size(); ++j) {
				Unit u = unit(ca[j]);
				int x0 = u.pos.i, y0 = u.pos.j;
				for (int x = x0-1; x <= x0+1; ++x) {
					for (int y = y0-1; y <= y0+1; ++y) {
						occupied[{x, y}]++;
					}
				}
			}
		}
		VI c = cars(me());
		int n = c.size();
		for (int i = 0; i < n; ++i) {
			Unit u = unit(c[i]);
			int x0 = u.pos.i, y0 = u.pos.j;
			int best = 8, value = -99999999;
			int x1 = x0, y1 = y0;
			for (int j = 0; j < 8; ++j) {
				int x = x0 + dx[j], y = y0 + dy[j];
				if (passable_c(x, y)) {
					int score = 0;
					Cell k = cell(x, y);
					if (k.type == Road) score += 10000;
					if (occupied[{x, y}] >= 2) score = -100000000;
					else if (j != 8 and k.id != -1) {
						if (unit(k.id).player == me()) score = -100000000;
						else score = 100000000;
					}
					else if (u.food < 30) score = dist_f[x][y];
					else score = dist_u[x][y];
					if (score > value) {
						best = j;
						value = score;
						x1 = x, y1 = y;
					}
				}
			}
			if (can_move(c[i]) and best != 8) {
				command(c[i], Dir(best));
				occupied[{x1, y1}]++;
			}
		}
		/*for (int id : C) {
			if (kind.find(id) == kind.end()) kind[id] = random(0, 1);
			if (can_move(id)) { // This also makes sense.
				if (kind[id] == 0) command(id, Dir(random(0, 7)));
				else command(id, Top);
			}
		}*/
	}


	/**
	* Play method, invoked once per each round.
	*/
	void play () {
	init();
	move_warriors();
	move_cars();
	}
};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
