from StateMachine import StateMachine, Alphabet
from StateMachine import Edge
from collections import deque


def classSplit(machine: StateMachine, partition: list, Splitter, splitSymbol):
    inSpltter = []
    noInSplitter = []
    for state in partition:
        flag = True
        for symbol in Splitter:
            if (symbol, splitSymbol) in machine.switches[state]:
                inSpltter.append(state)
                flag = False
                break
        if flag:
            noInSplitter.append(state)

    return inSpltter, noInSplitter


def findEquivalenceClasses(machine: StateMachine):
    Partition = [machine.endStates,
                 list(set([i for i in range(machine.numOfStates)]) -
                      set(machine.endStates))]

    Queue = deque()

    if len(machine.endStates) == machine.numOfStates:
        Partition.remove([])

    for symb in machine.V.listOfSymbols():
        for elem in Partition:
            Queue.append((elem, symb))

    while len(Queue) > 0:
        Splitter, splitSymbol = Queue.popleft()
        for currentPartition in Partition:
            inSpltter, noInSplitter = classSplit(machine=machine,
                                                 partition=currentPartition,
                                                 Splitter=Splitter,
                                                 splitSymbol=splitSymbol)

            if len(inSpltter) > 0 and len(noInSplitter) > 0:

                Partition.remove(currentPartition)
                Partition.append(inSpltter)
                Partition.append(noInSplitter)

                for symbol in machine.V.listOfSymbols():
                    Queue.append((inSpltter, symbol))
                    Queue.append((noInSplitter, symbol))

    return Partition


def minimizeStateMachine(machine: StateMachine) -> StateMachine:
    Partition = findEquivalenceClasses(machine)

    newStates = [0 for i in range(machine.numOfStates)]

    for indx, item in enumerate(Partition):
        for elem in item:
            newStates[elem] = indx

    newMachine = StateMachine(alphabet=machine.V,
                              numberOfStates=len(Partition),
                              begState=newStates[machine.begState],
                              endStates=[newStates[state] for state in machine.endStates])

    for stateFrom, state in enumerate(Partition):
        state = state[0]

        for stateTo, symb in machine.switches[state]:
            stateTo = newStates[stateTo]
            if (stateTo, symb) not in newMachine.switches[stateFrom]:
                newMachine.addEdge(stateFrom=stateFrom,
                                   stateTo=stateTo,
                                   symbol=symb)

    return newMachine
