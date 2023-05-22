class Duck:
    def sound(self):
        print("Quack!")


class Cow:
    def sound(self):
        print("Mooo!")


class Car:
    def sound(self):
        print("Honk honk!")


def listen(o):
    sound = getattr(o, 'sound', None)
    if callable(sound):
        sound()
    else:
        print("Could not invoke sound function")

def list_object(o):
    print("Class Name: ", type(o))
    print(dir(o))


def main():
    duck = Duck()
    cow = Cow()
    car = Car()

    listen(duck)
    listen(cow)
    listen(car)
    listen([])

    list_object(duck)
    list_object([])

if __name__=='__main__':
    main()
