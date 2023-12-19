#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <string>
using namespace std;

ifstream fin("input.txt");
ofstream fout("output.txt");

int main()
{
    int taksNr;
    fin >> taksNr;

    int nrStates;
    fin >> nrStates;

    int alphaLen;
    fin >> alphaLen;

    int nrFinalStates;
    fin >> nrFinalStates;

    int nrTransitions;
    fin >> nrTransitions;

    int initialState;
    fin >> initialState;
    initialState--;

    int finalStates[nrFinalStates];
    for (int i = 0; i < nrFinalStates; i++)
    {
        fin >> finalStates[i];
        finalStates[i]--;
    }

    vector<pair<int, char>> transitions[nrStates];
    vector<pair<int, char>> transitionsTransposed[nrStates];

    for (int i = 0; i < nrStates; i++)
    {
        for (int j = 0; j < alphaLen; j++)
        {
            int to;
            fin >> to;
            transitions[i].push_back({to - 1, 'a' + j});
            transitionsTransposed[to - 1].push_back({i, 'a' + j});
        }
    }

    if (taksNr == 1)
    {
        vector<vector<int>> mat(nrStates, vector<int>(nrStates + nrTransitions + 1, 0));
        mat[initialState][0] = 1;

        for (int step = 0; step <= nrStates + nrTransitions; step++)
        {
            for (int state = 0; state < nrStates; state++)
            {
                if (mat[state][step] == 1)
                {
                    for (auto transition : transitions[state])
                    {
                        mat[transition.first][step + 1] = 1;
                    }

                    if (step >= nrTransitions)
                    {
                        for (int i = 0; i < nrFinalStates; i++)
                        {
                            if (state == finalStates[i])
                            {
                                int currentNode = state;
                                std::string path = "";
                                for (int j = step; j >= 1; j--)
                                {
                                    for (auto transition : transitionsTransposed[currentNode])
                                    {
                                        if (mat[transition.first][j - 1] == 1)
                                        {
                                            path += transition.second;
                                            currentNode = transition.first;
                                            break;
                                        }
                                    }
                                }
                                reverse(path.begin(), path.end());
                                fout << path.size() << endl;
                                fout << path << endl;
                                return 0;
                            }
                        }
                    }
                }
            }
        }

        fout << -1 << endl;
    }

    return 0;
}