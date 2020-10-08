from StateMachine import StateMachine, Alphabet
from StateMachine import readStateMichineFromFile, saveStateMachineToText
from StateMachineViz import vizualizeStateMachine
from MinimizeStateMachine import minimizeStateMachine


def main():
    machine = readStateMichineFromFile('tests/machines/machine_5.txt')

    minMachine = minimizeStateMachine(machine)

    text = saveStateMachineToText(minMachine)
    print(text)

    vizualizeStateMachine(machine, 'before')
    vizualizeStateMachine(minMachine, 'after')

    pass


if __name__ == '__main__':
    main()
    pass
