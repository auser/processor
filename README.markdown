Processor
===

Processor was written to manage OS processes for [babysitter][0], which is a process manager for the project [Beehive][1]. It's usage is super simple. 

The `CombProcess` is a c++ class that allows for callbacks and restarts. For full usage, see test.cpp

    void callback(int p) {
      printf("callback called!\n");
    }
    
    int main (int argc, char const *argv[])
    {
      CombProcess p;            // CombProcess object

      p.set_callback(callback); // Set a callback function to be called upon death
      p.set_secs(2);            // Set the number of seconds between `polling` the pid
      p.set_micro(2);           // Set the number of microseconds
      p.set_nano(2);            // Set the number of nanoseconds between polling

      const char* env[] = { "NAME=bob", NULL }; // Setup some environment variables

      argv++; // We don't want our program name to be included

      p.monitored_start(argc-1, argv, (char **) env); // Run the program
      return 0;
    }
    
This test program above will run the function that you specify in argv (as argv[1]). It will call the callback function above since it's specified. If it's not specified, then it will simply restart the program you specify.

    s test/comb_test1.sh
    s test/comb_test2.sh
    
    
Author
------

Ari Lerner (auser) arilerner@mac.com

[0]: http://github.com/auser/babysitter
[1]: http://github.com/auser/beehive