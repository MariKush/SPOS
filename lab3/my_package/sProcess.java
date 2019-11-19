package my_package;

public class sProcess {
  
  public int cputime;
  public int ioblocking;
  public int cpudone;
  public int ionext;
  public int numblocked;
  public int countTicket;

  public sProcess (int cputime, int ioblocking, int cpudone, int ionext, int numblocked, int countTicket) {
    this.cputime = cputime;
    this.ioblocking = ioblocking;
    this.cpudone = cpudone;
    this.ionext = ionext;
    this.numblocked = numblocked;
    this.countTicket = countTicket;
  } 	
}
