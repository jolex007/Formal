
class Alphabet:
    def __init__(self, alphabet: list):
        self.alphabet = alphabet
        pass
    
    def contains(self, symbol) -> bool:
        try:
            self.alphabet.index(symbol)
        except ValueError:
            return False
        else:
            return True

    def listOfSymbols(self) -> list:
        return self.alphabet
    pass


class StateMachine:
    def __init__(self, alphabet: Alphabet, 
                       numberOfStates: int, 
                       begState: int,
                       endStates: list,
                       switches=[]):
        self.V = alphabet
        self.numOfStates = numberOfStates
        self.begState = begState
        self.endStates = endStates

        if len(switches) == 0:
            self.switches = [[] for i in range(self.numOfStates)]
        else:
            self.switches = switches

        pass

    def addEdge(self, stateFrom, stateTo, symbol):
        self.switches[stateFrom].append((stateTo, symbol))
    pass

def readStateMichineFromFile(filename: str) -> StateMachine:
    
    f = open(filename)
    for indx, line in enumerate(f):
        line = line.replace('\n', '')
        if indx == 0:
            curr_list = line.split()
            alphabet = Alphabet(curr_list)
        elif indx == 1:
            numOfStates = int(line)
        elif indx == 2:
            begState = int(line)
        elif indx == 3:
            endStates = line.split()
            endStates = list(map(int, endStates))

            machine = StateMachine(alphabet=alphabet,
                                   numberOfStates=numOfStates,
                                   begState=begState,
                                   endStates=endStates)
        elif indx >= 4:
            stateFrom, stateTo, symbol = map(str, line.split())
            stateTo = int(stateTo)
            stateFrom = int(stateFrom)
            
            machine.addEdge(stateFrom, stateTo, symbol)
    
    return machine

def saveStateMachineToText(machine: StateMachine) -> str:
    answer = ''

    answer += ' '.join(machine.V.listOfSymbols()) + '\n'
    answer += str(machine.numOfStates) + '\n'
    answer += str(machine.begState) + '\n'
    answer += ' '.join(list(map(str, machine.endStates))) + '\n'
    
    for stateFrom, curr_list in enumerate(machine.switches):
        for stateTo, symbol in curr_list:
            answer += str(stateFrom) + ' ' + str(stateTo) + ' ' + symbol + '\n'

    return answer       
