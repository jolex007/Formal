import pytest
from StateMachine import StateMachine, Alphabet
from StateMachine import readStateMichineFromFile, saveStateMachineToText
from MinimizeStateMachine import minimizeStateMachine


class TestStateMachine:
    @pytest.mark.parametrize('machine_filename, answer_filename', [
        ('tests/machines/machine_1.txt', 'tests/machines/machine_2.txt'),
        ('tests/machines/machine_3.txt', 'tests/machines/machine_4.txt')
    ])
    def test_minimizer(self, machine_filename: str, answer_filename: str):
        machine = readStateMichineFromFile(machine_filename)

        minMachine = minimizeStateMachine(machine)

        minMachineText = saveStateMachineToText(minMachine)

        f = open(answer_filename)
        assert minMachineText == f.read()
        pass
