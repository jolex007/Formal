from StateMachine import StateMachine, Alphabet
from StateMachineViz import vizualizeStateMachine
from MinimizeStateMachine import minimizeStateMachine

def main():
    machine = StateMachine(alphabet=Alphabet(['0', '1']),
                           numberOfStates=8,
                           begState=0,
                           endStates=[5, 6],
                           switches=[[(7, '0'), (1, '1')],
                                     [(7, '0'), (0, '1')],
                                     [(4, '0'), (5, '1')],
                                     [(4, '0'), (5, '1')],
                                     [(5, '0'), (6, '1')],
                                     [(5, '0'), (5, '1')],
                                     [(6, '0'), (5, '1')],
                                     [(2, '0'), (1, '1')]]
                                     )

    minMachine = minimizeStateMachine(machine)

    vizualizeStateMachine(machine, 'before')
    vizualizeStateMachine(minMachine, 'after')

    pass

if __name__ == '__main__':
    main()
    pass