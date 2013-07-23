#include "main.h"
#include "glutwin.h"
#include "gamewin.h"

int main(int ac, char** av) {
	bool stereo = false;
	int sep = 40;
	std::string str = "temp.lvl";
	if (ac > 1) {
		for (int i = 1; i < ac; ++i) {
			if (!strcmp(av[i], "-s"))
				stereo = true;
			if (!strcmp(av[i], "-l"))
				if ((i + 1) < ac)
					str = av[i + 1];
			if (!strcmp(av[i], "-c"))
				if ((i + 1) < ac)
					sep = atoi(av[i + 1]);
		}
	}
	gamewin game(stereo, str, (float)sep);
#ifdef _DEBUG
	glutwin win(ac, av, &game);
#else
	glutwin win(ac, av, &game, true);
#endif // _DEBUG
	win.run();
}