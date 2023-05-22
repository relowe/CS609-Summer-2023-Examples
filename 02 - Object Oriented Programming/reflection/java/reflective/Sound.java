import java.lang.reflect.*;

public class Sound {
    public static void listen(Object o) {
        try {
            Class cls = o.getClass();
            Method m = cls.getMethod("sound");
            m.invoke(o);
        } catch(Exception e) {
            System.out.println("Could not invoke sound.");
        }
    }


    public static void listObject(Object o) {
        Class cls = o.getClass();

        System.out.printf("Class Name: %s\n", cls.getCanonicalName());
        System.out.println("Fields");
        for(Field f : cls.getFields()) {
            System.out.println(f.getName());
        }
        System.out.println("Methods");
        for(Method m : cls.getMethods()) {
            System.out.println(m.getName());
        }
    }


    public static void main(String [] args) {
        Duck duck = new Duck();
        Cow cow   = new Cow();
        Car car   = new Car();

        listen(duck);
        listen(cow);
        listen(car);
        listen(args);

        listObject(duck);
        listObject(args);
    }
}
