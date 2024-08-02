from pyg4ometry import geant4

reg = geant4.Registry()
world = geant4.solid.Box("world", 20, 20, 20, reg, "m")