from StateMachine import StateMachine, Alphabet
from collections import deque

def classSplit(machine: StateMachine, R: list, C, a):
    R1 = []
    R2 = []
    for state in R:
        flag = True
        for c in C:
            if (c, a) in machine.switches[state]:
                R1.append(state)
                flag = False
                break
        if flag:
            R2.append(state)
    
    return R1, R2

def findEquivalenceClasses(machine: StateMachine):
    test_set = set([i for i in range(machine.numOfStates)]) - set(machine.endStates)
    P = [machine.endStates, list(test_set)]
    S = deque()

    for symb in machine.V.listOfSymbols():
        for elem in P:
            S.append((elem, symb))
    
    print(len(S))
    while len(S) > 0:
        C, a = S.popleft()
        for R in P:
            R1, R2 = classSplit(machine, R, C, a)

            if len(R1) > 0 and len(R2) > 0:

                P.remove(R)
                P.append(R1)
                P.append(R2)

                for c in machine.V.listOfSymbols():
                    S.append((R1, c))
                    S.append((R2, c))
    
    return P

def minimizeStateMachine(machine: StateMachine) -> StateMachine:
    P = findEquivalenceClasses(machine)

    newStates = [0 for i in range(machine.numOfStates)]

    for indx, item in enumerate(P):
        for elem in item:
            newStates[elem] = indx
    
    newMachine = StateMachine(alphabet=machine.V,
                              numberOfStates=len(newStates),
                              begState=newStates[machine.begState],
                              endStates=[newStates[state] for state in machine.endStates])
    
    for stateFrom, state in enumerate(P):
        state = state[0]
        
        for stateTo, symb in machine.switches[state]:
            stateTo = newStates[stateTo]
            if (stateTo, symb) not in newMachine.switches[stateFrom]:
                newMachine.switches[stateFrom].append((stateTo, symb))
    
    return newMachine

