#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <string>
#include <map>
#include <math.h>
#include <set>
#define N 201
using namespace std;

ifstream fin("input.txt");
ofstream fout("output.txt");
map<int, string> stateToPath[N][N];

vector<int> powersOf2(
    int n)
{
    vector<int> powers;
    int counter = 0;
    while (n > 0)
    {
        if (n % 2 == 1)
        {
            powers.push_back(counter);
        }
        counter++;
        n /= 2;
    }
    return powers;
}

string pathReconstruction(
    int start,
    int end,
    int powerOf2,
    vector<pair<int, char>> transitions[],
    vector<vector<vector<int>>> hopsByPowerOF2)
{
    if (stateToPath[start][end].count(powerOf2) > 0)
    {
        return stateToPath[start][end][powerOf2];
    }

    if (powerOf2 == 0)
    {
        for (auto transition : transitions[start])
        {
            if (transition.first == end)
            {
                return string(1, transition.second);
            }
        }
    }

    for (auto transition : hopsByPowerOF2[start][powerOf2 - 1])
    {
        for (auto transition2 : hopsByPowerOF2[transition][powerOf2 - 1])
        {
            if (transition2 == end)
            {
                stateToPath[start][end][powerOf2] =
                    pathReconstruction(start, transition, powerOf2 - 1, transitions, hopsByPowerOF2) +
                    pathReconstruction(transition, end, powerOf2 - 1, transitions, hopsByPowerOF2);

                return stateToPath[start][end][powerOf2];
            }
        }
    }

    return "";
}

void readInputValues(
    int &tasknr,
    int &nrStates,
    int &alphaLen,
    int &nrFinalStates,
    int &nrTransitions,
    int &initialState)
{
    fin >> tasknr;
    fin >> nrStates;
    fin >> alphaLen;
    fin >> nrFinalStates;
    fin >> nrTransitions;
    fin >> initialState;
    initialState--;
}

void readInputVectors(
    int nrStates,
    int alphaLen,
    int nrFinalStates,
    int finalStates[],
    vector<pair<int, char>> transitions[],
    vector<pair<int, char>> transitionsTransposed[])
{
    for (int i = 0; i < nrFinalStates; i++)
    {
        fin >> finalStates[i];
        finalStates[i]--;
    }

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
}

void PrintAnswerTask1(
    int state,
    int step,
    vector<vector<int>> dpReachNodeInSteps,
    vector<pair<int, char>> transitionsTransposed[])
{
    int currentNode = state;
    std::string path = "";
    for (int j = step; j >= 1; j--)
    {
        for (auto transition : transitionsTransposed[currentNode])
        {
            if (dpReachNodeInSteps[transition.first][j - 1] == 1)
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
}

void solveTask1(
    int nrStates,
    int nrFinalStates,
    int nrTransitions,
    int finalStates[],
    vector<pair<int, char>> transitions[],
    vector<pair<int, char>> transitionsTransposed[],
    int initialState)
{
    vector<vector<int>> dpReachNodeInSteps(nrStates, vector<int>(nrStates + nrTransitions + 1, 0));
    dpReachNodeInSteps[initialState][0] = 1;

    for (int step = 0; step <= nrStates + nrTransitions; step++)
    {
        for (int state = 0; state < nrStates; state++)
        {
            if (dpReachNodeInSteps[state][step] == 1)
            {
                for (auto transition : transitions[state])
                {
                    dpReachNodeInSteps[transition.first][step + 1] = 1;
                }

                if (step >= nrTransitions)
                {
                    for (int i = 0; i < nrFinalStates; i++)
                    {
                        if (state == finalStates[i])
                        {
                            PrintAnswerTask1(state, step, dpReachNodeInSteps, transitionsTransposed);
                            return;
                        }
                    }
                }
            }
        }
    }

    fout << -1 << endl;
}

void InitialiseHopesByPowerOf2(
    int nrStates,
    vector<pair<int, char>> transitions[],
    vector<vector<vector<int>>> &hopsByPowerOF2)
{
    for (int i = 0; i < nrStates; i++)
    {
        for (auto transition : transitions[i])
        {
            hopsByPowerOF2[i][0].push_back(transition.first);
        }
    }
}

void PopulateHopesByPowerOf2(
    int nrStates,
    int nrTransitions,
    vector<vector<vector<int>>> &hopsByPowerOF2)
{
    for (int power = 1; power <= log2(nrTransitions); power++)
    {
        for (int state = 0; state < nrStates; state++)
        {
            map<int, int> visited;
            for (auto transition : hopsByPowerOF2[state][power - 1])
            {
                for (auto transition2 : hopsByPowerOF2[transition][power - 1])
                {
                    if (visited.count(transition2) > 0)
                    {
                        continue;
                    }

                    visited[transition2] = 1;
                    hopsByPowerOF2[state][power].push_back(transition2);
                }
            }
        }
    }
}

void CreateCurrentStates(
    int initialState,
    vector<int> nrTransitionsDecomposed,
    vector<vector<vector<int>>> &hopsByPowerOF2,
    set<int> &currentStates)
{
    currentStates.insert(initialState);
    for (int i = 0; i < (int)(nrTransitionsDecomposed.size()); i++)
    {
        set<int> nextStates;
        for (auto state : currentStates)
        {
            for (auto nextState : hopsByPowerOF2[state][nrTransitionsDecomposed[i]])
            {
                nextStates.insert(nextState);
            }
        }
        currentStates = nextStates;
    }
}

string CreatePathAfterNrTransitions(
    int step,
    int &currentNode,
    vector<vector<int>> dpReachNodeInSteps,
    vector<pair<int, char>> transitionsTransposed[])
{
    string pathAfterNrTransitions = "";
    for (int j = step; j >= 1; j--)
    {
        for (auto transition : transitionsTransposed[currentNode])
        {
            if (dpReachNodeInSteps[transition.first][j - 1] == 1)
            {
                pathAfterNrTransitions += transition.second;
                currentNode = transition.first;
                break;
            }
        }
    }
    reverse(pathAfterNrTransitions.begin(), pathAfterNrTransitions.end());

    return pathAfterNrTransitions;
}

vector<string> CreatePathBeforeNrTransitions(
    int nrStates,
    int initialState,
    int &currentNode,
    vector<int> nrTransitionsDecomposed,
    vector<vector<vector<int>>> hopsByPowerOF2,
    vector<pair<int, char>> transitions[])
{
    vector<string> pathBeforeNrTransitions;
    for (int j = nrTransitionsDecomposed.size() - 1; j >= 0; j--)
    {
        int powerOf2 = nrTransitionsDecomposed[j];
        bool broken = false;
        for (int node = 0; node < nrStates && !broken; node++)
        {
            for (auto transition : hopsByPowerOF2[node][powerOf2])
            {
                if (transition == currentNode)
                {
                    if (j == 0)
                    {
                        if (node == initialState)
                        {
                            pathBeforeNrTransitions.push_back(pathReconstruction(node, currentNode, powerOf2, transitions, hopsByPowerOF2));
                            currentNode = node;
                            broken = true;
                            break;
                        }
                    }
                    else
                    {
                        pathBeforeNrTransitions.push_back(pathReconstruction(node, currentNode, powerOf2, transitions, hopsByPowerOF2));
                        currentNode = node;
                        broken = true;
                        break;
                    }
                }
            }
        }
    }

    return pathBeforeNrTransitions;
}

string CreateTotalPath(
    string pathAfterNrTransitions,
    vector<string> pathBeforeNrTransitions)
{
    string totalPath = "";
    for (int i = pathBeforeNrTransitions.size() - 1; i >= 0; i--)
    {
        totalPath += pathBeforeNrTransitions[i];
    }
    totalPath += pathAfterNrTransitions;

    return totalPath;
}

void SolveTask2(
    int nrStates,
    int nrFinalStates,
    int nrTransitions,
    int finalStates[],
    vector<pair<int, char>> transitions[],
    vector<pair<int, char>> transitionsTransposed[],
    int initialState)
{
    vector<vector<vector<int>>> hopsByPowerOF2(nrStates, vector<vector<int>>(log2(nrTransitions) + 1));
    vector<int> nrTransitionsDecomposed = powersOf2(nrTransitions);
    vector<vector<int>> dpReachNodeInSteps(nrStates, vector<int>(nrStates + 1, 0));
    set<int> currentStates;

    InitialiseHopesByPowerOf2(nrStates, transitions, hopsByPowerOF2);
    PopulateHopesByPowerOf2(nrStates, nrTransitions, hopsByPowerOF2);
    CreateCurrentStates(
        initialState,
        nrTransitionsDecomposed,
        hopsByPowerOF2,
        currentStates);

    for (auto state : currentStates)
    {
        dpReachNodeInSteps[state][0] = 1;
    }

    for (int step = 0; step <= nrStates; step++)
    {
        for (int state = 0; state < nrStates; state++)
        {
            if (dpReachNodeInSteps[state][step] == 1)
            {
                for (auto transition : transitions[state])
                {
                    dpReachNodeInSteps[transition.first][step + 1] = 1;
                }

                for (int i = 0; i < nrFinalStates; i++)
                {
                    if (state == finalStates[i])
                    {
                        int currentNode = state;
                        std::string pathAfterNrTransitions =
                            CreatePathAfterNrTransitions(
                                step,
                                currentNode,
                                dpReachNodeInSteps,
                                transitionsTransposed);

                        vector<string> pathBeforeNrTransitions =
                            CreatePathBeforeNrTransitions(
                                nrStates,
                                initialState,
                                currentNode,
                                nrTransitionsDecomposed,
                                hopsByPowerOF2,
                                transitions);

                        string totalPath = CreateTotalPath(
                            pathAfterNrTransitions,
                            pathBeforeNrTransitions);

                        fout << totalPath.size() << endl;
                        fout << totalPath << endl;
                        return;
                    }
                }
            }
        }
    }

    fout << -1 << endl;
}

int main()
{
    int taksNr;
    int nrStates;
    int alphaLen;
    int nrFinalStates;
    int nrTransitions;
    int initialState;

    readInputValues(
        taksNr,
        nrStates,
        alphaLen,
        nrFinalStates,
        nrTransitions,
        initialState);

    int finalStates[nrFinalStates] = {};
    vector<pair<int, char>> transitions[nrStates] = {};
    vector<pair<int, char>> transitionsTransposed[nrStates] = {};

    readInputVectors(
        nrStates,
        alphaLen,
        nrFinalStates,
        finalStates,
        transitions,
        transitionsTransposed);

    switch (taksNr)
    {
    case 1:
        solveTask1(
            nrStates,
            nrFinalStates,
            nrTransitions,
            finalStates,
            transitions,
            transitionsTransposed,
            initialState);
        break;
    case 2:
        SolveTask2(
            nrStates,
            nrFinalStates,
            nrTransitions,
            finalStates,
            transitions,
            transitionsTransposed,
            initialState);
        break;
    default:
        break;
    }

    return 0;
}