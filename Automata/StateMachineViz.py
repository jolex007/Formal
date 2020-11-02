from graphviz import Digraph
from StateMachine import StateMachine, Alphabet


def vizualizeStateMachine(machine: StateMachine, name: str):
    f = Digraph(name, filename=name)
    f.attr(rankdir='LR', size='8,5')

    f.attr('node', shape='doublecircle')
    for state in machine.endStates:
        f.node(str(state))

    f.attr('node', shape='circle')

    for stateFrom in range(machine.numOfStates):
        for stateTo, char in machine.switches[stateFrom]:
            f.edge(str(stateFrom), str(stateTo), label=char)

    f.attr('node', style='invisible')
    f.node("secret_node")
    f.edge("secret_node", str(machine.begState))

    f.view()

    pass
