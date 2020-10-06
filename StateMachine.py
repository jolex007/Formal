
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
    


    pass