import java.lang.reflect.*;

public class Sound {
    public static void listen(Soundable o) {
        o.sound();
    }



    public static void main(String [] args) {
        Duck duck = new Duck();
        Cow cow   = new Cow();
        Car car   = new Car();

        listen(duck);
        listen(cow);
        listen(car);
        // Can't do this one thanks to compile-time checking ---> listen(args);

    }
}
