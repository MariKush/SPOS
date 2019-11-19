// Run() is called from Scheduling.main() and is where
// the scheduling algorithm written by the user resides.
// User modification should occur within the Run() function.

package my_package;

import java.util.Random;
import java.util.Vector;
import java.io.*;

public class SchedulingAlgorithm {

  static Random random;

  static{
    random = new Random(System.currentTimeMillis());;
  }

  private static int nextProcess( Vector<sProcess> processVector, int blocked){

    Vector<Integer> process = new Vector<Integer>();
    int size = processVector.size();
    for(int i = 0; i < size; i++){
      if(i == blocked)
        continue;
      if(processVector.elementAt(i).cpudone!=processVector.elementAt(i).cputime)
        process.addElement(i);
    }
    if (process.size() == 0) process.addElement(blocked);

    size = process.size();
    int countTicket = 10*size;
    int []ticket = new int[countTicket];
    for(int i = 0; i < countTicket; i++){
      ticket[i] = process.elementAt(i%size);
    }
    return ticket[random.nextInt(countTicket)];

  }

  public static Results Run(int runtime, Vector<sProcess> processVector, Results result) {
    int i = 0;
    int comptime = 0;
    int currentProcess = 0;
    int size = processVector.size();
    int completed = 0;
    String resultsFile = "Summary-Processes";

    result.schedulingType = "Batch (Nonpreemptive)";
    result.schedulingName = "Lottery"; 
    try {
      //BufferedWriter out = new BufferedWriter(new FileWriter(resultsFile));
      //OutputStream out = new FileOutputStream(resultsFile);
      PrintStream out = new PrintStream(new FileOutputStream(resultsFile));
      sProcess process = processVector.elementAt(currentProcess);
      out.println("Process: " + currentProcess + " registered... (" + process.cputime + " " + process.ioblocking + " " + process.cpudone + ")");
      while (comptime < runtime) {
        if (process.cpudone == process.cputime) {
          completed++;
          out.println("Process: " + currentProcess + " completed... (" + process.cputime + " " + process.ioblocking + " " + process.cpudone + ")");
          if (completed == size) {
            result.compuTime = comptime;
            out.close();
            return result;
          }

          currentProcess = nextProcess(processVector, -1);

          process = processVector.elementAt(currentProcess);
          out.println("Process: " + currentProcess + " registered... (" + process.cputime + " " + process.ioblocking + " " + process.cpudone + ")");
        }      
        if (process.ioblocking == process.ionext) {
          out.println("Process: " + currentProcess + " I/O blocked... (" + process.cputime + " " + process.ioblocking + " " + process.cpudone + ")");
          process.numblocked++;
          process.ionext = 0; 
          
          currentProcess = nextProcess(processVector, currentProcess);

          process = processVector.elementAt(currentProcess);
          out.println("Process: " + currentProcess + " registered... (" + process.cputime + " " + process.ioblocking + " " + process.cpudone + ")");
        }        
        process.cpudone++;       
        if (process.ioblocking > 0) {
          process.ionext++;
        }
        comptime++;
      }
      out.close();
    } catch (IOException e) { /* Handle exceptions */ }
    result.compuTime = comptime;
    return result;
  }
}
