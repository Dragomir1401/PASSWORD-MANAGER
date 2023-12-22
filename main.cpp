#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <string>
#include <map>
#include <math.h>
#include <set>
using namespace std;

// Maximum number of states for the task 2
#define N 201

// Input file stream and output file stream
ifstream fin("input.txt");
ofstream fout("output.txt");

// Map that stores the path from a state to another state
map<int, string> stateToPath[N][N];

// Returns the powers of 2 that sum up to n
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

// Returns the path from a state to another state, start and end are the states,
// powerOf2 is the number of transitions
string pathReconstruction(
    int start,
    int end,
    int powerOf2,
    vector<pair<int, char>> transitions[],
    vector<vector<vector<int>>> hopsByPowerOF2)
{
    // If the path is already computed, return it
    if (stateToPath[start][end].count(powerOf2) > 0)
    {
        return stateToPath[start][end][powerOf2];
    }

    // If the transition is immediate, return the transition
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

    // If the transition is not immediate, try to find a transition that
    // goes from start to end
    for (auto transition : hopsByPowerOF2[start][powerOf2 - 1])
    {
        for (auto transition2 : hopsByPowerOF2[transition][powerOf2 - 1])
        {
            if (transition2 == end)
            {
                // Recursively compute the path from start to transition and
                // from transition to end
                stateToPath[start][end][powerOf2] =
                    pathReconstruction(start, transition, powerOf2 - 1, transitions, hopsByPowerOF2) +
                    pathReconstruction(transition, end, powerOf2 - 1, transitions, hopsByPowerOF2);

                return stateToPath[start][end][powerOf2];
            }
        }
    }

    // If there is no path from start to end, return an empty string
    return "";
}

// Reads the input values
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

// Reads the input vectors
void readInputVectors(
    int nrStates,
    int alphaLen,
    int nrFinalStates,
    int finalStates[],
    vector<pair<int, char>> transitions[],
    vector<pair<int, char>> transitionsTransposed[])
{
    // Read the final states
    for (int i = 0; i < nrFinalStates; i++)
    {
        fin >> finalStates[i];
        finalStates[i]--;
    }

    // Read the transitions and the transposed transitions
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

// Prints the answer for task 1
void PrintAnswerTask1(
    int state,
    int step,
    vector<vector<int>> dpReachNodeInSteps,
    vector<pair<int, char>> transitionsTransposed[])
{
    int currentNode = state;
    std::string path = "";

    // Reconstruct the path
    for (int j = step; j >= 1; j--)
    {
        for (auto transition : transitionsTransposed[currentNode])
        {
            // If the transition is valid, add it to the path
            if (dpReachNodeInSteps[transition.first][j - 1] == 1)
            {
                path += transition.second;
                currentNode = transition.first;
                break;
            }
        }
    }

    // Reverse the path as it was constructed backwards
    reverse(path.begin(), path.end());

    // Print the answer
    fout << path.size() << endl;
    fout << path << endl;
}

// Solution driver for task 1
void solveTask1(
    int nrStates,
    int nrFinalStates,
    int nrTransitions,
    int finalStates[],
    vector<pair<int, char>> transitions[],
    vector<pair<int, char>> transitionsTransposed[],
    int initialState)
{
    // Dp matrix that stores if a state can be reached in a certain number of steps
    vector<vector<int>> dpReachNodeInSteps(nrStates, vector<int>(nrStates + nrTransitions + 1, 0));
    dpReachNodeInSteps[initialState][0] = 1;

    for (int step = 0; step <= nrStates + nrTransitions; step++)
    {
        for (int state = 0; state < nrStates; state++)
        {
            // If the state can be reached in step steps, try to reach the next states
            if (dpReachNodeInSteps[state][step] == 1)
            {
                for (auto transition : transitions[state])
                {
                    dpReachNodeInSteps[transition.first][step + 1] = 1;
                }

                // If the condition for at least nrTransitions steps is met, print the answer
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

    // If there is no answer, print -1
    fout << -1 << endl;
}

// Initialises the hopsByPowerOF2 matrix
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

// Puts necessary transitions in the hopsByPowerOF2 matrix
void PopulateHopesByPowerOf2(
    int nrStates,
    int nrTransitions,
    vector<vector<vector<int>>> &hopsByPowerOF2)
{
    // For each power of 2
    for (int power = 1; power <= log2(nrTransitions); power++)
    {
        for (int state = 0; state < nrStates; state++)
        {
            // Hold the visited states
            map<int, int> visited;

            // For each power of 2 transition
            for (auto transition : hopsByPowerOF2[state][power - 1])
            {
                for (auto transition2 : hopsByPowerOF2[transition][power - 1])
                {
                    // If the transition was already visited, skip it
                    if (visited.count(transition2) > 0)
                    {
                        continue;
                    }

                    // Add the transition to the visited states
                    visited[transition2] = 1;

                    // Add the transition to the hopsByPowerOF2 matrix
                    hopsByPowerOF2[state][power].push_back(transition2);
                }
            }
        }
    }
}

// Creates the current states based on accesible states
void CreateCurrentStates(
    int initialState,
    vector<int> nrTransitionsDecomposed,
    vector<vector<vector<int>>> &hopsByPowerOF2,
    set<int> &currentStates)
{
    // Add the initial state to the current states
    currentStates.insert(initialState);

    // For each power of 2 transition
    for (int i = 0; i < (int)(nrTransitionsDecomposed.size()); i++)
    {
        set<int> nextStates;

        // For each state in the current states
        for (auto state : currentStates)
        {
            for (auto nextState : hopsByPowerOF2[state][nrTransitionsDecomposed[i]])
            {
                // Add the next state to the next states
                nextStates.insert(nextState);
            }
        }

        // Go further with the next states
        currentStates = nextStates;
    }
}

// Creates path with states after the condition for at least nrTransitions steps is met
string CreatePathAfterNrTransitions(
    int step,
    int &currentNode,
    vector<vector<int>> dpReachNodeInSteps,
    vector<pair<int, char>> transitionsTransposed[])
{
    string pathAfterNrTransitions = "";

    // Reconstruct the path
    for (int j = step; j >= 1; j--)
    {
        for (auto transition : transitionsTransposed[currentNode])
        {
            if (dpReachNodeInSteps[transition.first][j - 1] == 1)
            {
                // Add the transition to the path
                pathAfterNrTransitions += transition.second;
                currentNode = transition.first;
                break;
            }
        }
    }

    // Reverse the path as it was constructed backwards
    reverse(pathAfterNrTransitions.begin(), pathAfterNrTransitions.end());

    return pathAfterNrTransitions;
}

// Creates path with states before the condition for at least nrTransitions steps is met
vector<string> CreatePathBeforeNrTransitions(
    int nrStates,
    int initialState,
    int &currentNode,
    vector<int> nrTransitionsDecomposed,
    vector<vector<vector<int>>> hopsByPowerOF2,
    vector<pair<int, char>> transitions[],
    vector<vector<vector<int>>> parents)
{
    vector<string> pathBeforeNrTransitions;

    // Create the path before the nrTransitions
    set<int> currentStates;

    // Add the initial state to the current states
    currentStates.insert(currentNode);

    // For each power of 2 transition
    for (int j = nrTransitionsDecomposed.size() - 1; j >= 0; j--)
    {
        set<int> nextStates;
        int powerOf2 = nrTransitionsDecomposed[j];

        for (auto state : currentStates)
        {
            for (int node = 0; node < nrStates; node++)
            {
                for (auto transition : hopsByPowerOF2[node][powerOf2])
                {
                    // If the transition is valid, add the node to the next states
                    if (transition == state)
                    {
                        // Add the node to the next states
                        nextStates.insert(node);

                        // Hold the parent of the node
                        parents[node][j].push_back(state);
                    }
                }
            }
        }

        // Advance to the next states
        currentStates = nextStates;
    }

    // Start from the initial state
    currentNode = initialState;

    // For each power of 2 transition
    for (int i = 0; i < (int)(nrTransitionsDecomposed.size()); i++)
    {
        // Add the path to the path before nrTransitions
        pathBeforeNrTransitions.push_back(pathReconstruction(currentNode, parents[currentNode][i][0], nrTransitionsDecomposed[i], transitions, hopsByPowerOF2));

        // Go to the parent of the current node
        currentNode = parents[currentNode][i][0];
    }

    return pathBeforeNrTransitions;
}

// Creates the total path
string CreateTotalPath(
    string pathAfterNrTransitions,
    vector<string> pathBeforeNrTransitions)
{
    string totalPath = "";

    // Add all components of the path before nrTransitions
    for (int i = 0; i < (int)(pathBeforeNrTransitions.size()); i++)
    {
        totalPath += pathBeforeNrTransitions[i];
    }

    // Add the path after nrTransitions
    totalPath += pathAfterNrTransitions;

    return totalPath;
}

// Solution driver for task 2
void SolveTask2(
    int nrStates,
    int nrFinalStates,
    int nrTransitions,
    int finalStates[],
    vector<pair<int, char>> transitions[],
    vector<pair<int, char>> transitionsTransposed[],
    int initialState)
{
    // Matrix that stores the hops by power of 2 technique
    vector<vector<vector<int>>> hopsByPowerOF2(nrStates, vector<vector<int>>(log2(nrTransitions) + 1));

    // Decompose the number of transitions in powers of 2
    vector<int> nrTransitionsDecomposed = powersOf2(nrTransitions);

    // Matrix that stores if a state can be reached in a certain number of steps
    vector<vector<int>> dpReachNodeInSteps(nrStates, vector<int>(nrStates + 1, 0));

    // Matrix that stores the parents of a node
    vector<vector<vector<int>>> parents(nrStates, vector<vector<int>>(log2(nrTransitions) + 1));

    // Set that stores the current states
    set<int> currentStates;

    // Initialise the hopsByPowerOF2 matrix
    InitialiseHopesByPowerOf2(nrStates, transitions, hopsByPowerOF2);

    // Populate the hopsByPowerOF2 matrix
    PopulateHopesByPowerOf2(nrStates, nrTransitions, hopsByPowerOF2);

    // Create the current states
    CreateCurrentStates(
        initialState,
        nrTransitionsDecomposed,
        hopsByPowerOF2,
        currentStates);

    // Initialise the dp matrix
    for (auto state : currentStates)
    {
        dpReachNodeInSteps[state][0] = 1;
    }

    // For each step
    for (int step = 0; step <= nrStates; step++)
    {
        for (int state = 0; state < nrStates; state++)
        {
            // If the state can be reached in step steps, try to reach the next states
            if (dpReachNodeInSteps[state][step] == 1)
            {
                // Add the next states to the dp matrix
                for (auto transition : transitions[state])
                {
                    dpReachNodeInSteps[transition.first][step + 1] = 1;
                }

                // If the state is a final state, print the answer
                for (int i = 0; i < nrFinalStates; i++)
                {
                    if (state == finalStates[i])
                    {
                        // Update current node
                        int currentNode = state;

                        // Create the path after nrTransitions
                        std::string pathAfterNrTransitions =
                            CreatePathAfterNrTransitions(
                                step,
                                currentNode,
                                dpReachNodeInSteps,
                                transitionsTransposed);

                        // Create the path before nrTransitions
                        vector<string> pathBeforeNrTransitions =
                            CreatePathBeforeNrTransitions(
                                nrStates,
                                initialState,
                                currentNode,
                                nrTransitionsDecomposed,
                                hopsByPowerOF2,
                                transitions,
                                parents);

                        // Compute the total path
                        string totalPath = CreateTotalPath(
                            pathAfterNrTransitions,
                            pathBeforeNrTransitions);

                        // Print the actual answer
                        fout << totalPath.size() << endl;
                        fout << totalPath << endl;

                        // Break execution
                        return;
                    }
                }
            }
        }
    }

    // If there is no answer, print -1
    fout << -1 << endl;
}

int main()
{
    // Input values
    int taksNr;
    int nrStates;
    int alphaLen;
    int nrFinalStates;
    int nrTransitions;
    int initialState;

    // Read the input values
    readInputValues(
        taksNr,
        nrStates,
        alphaLen,
        nrFinalStates,
        nrTransitions,
        initialState);

    // Input vectors
    int finalStates[nrFinalStates] = {};
    vector<pair<int, char>> transitions[nrStates] = {};
    vector<pair<int, char>> transitionsTransposed[nrStates] = {};

    // Read the input vectors
    readInputVectors(
        nrStates,
        alphaLen,
        nrFinalStates,
        finalStates,
        transitions,
        transitionsTransposed);

    // Solve the task based on the task number
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