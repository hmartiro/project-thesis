/* 

shows examples on how to use libepos 


*/


#include "epos.h"



#define ZERO_OFFSET 452700    // steps between reference point and 1st filter
#define FILTER_FILTER 666667  // steps between 2 filters
#define DEBUG


/*
*********************** 
*** main program *** 
***********************
*/


int main(void){
  
  char name[128];
  int  n;
  WORD w = 0x0, estatus=0x0;  
  //  WORD dw[2] = {0x0, 0x0};
  

  printf("\n   *** basic test of EPOS communication ***\n\n");
  printf("\n an alternating '/', '\\' char means that the program is waiting\n");
  printf(" for the EPOS to respond.\n");

  
#ifdef DEBUG
  printf("lines starting with '>>' is data send to EPOS,\n");
  printf("lines starting with '<<' is data received from EPOS.\n");
  printf("Intermediate 'ACK' are NOT shown!\n\n");
#endif
  

  
  /* open EPOS device */
  if ( openEPOS("/dev/ttyS0") < 0) exit(-1);



  /*  read manufactor device name */
  if ( readDeviceName(name) < 0) {
    fprintf(stderr, "ERROR: cannot read device name!\n");
  }
  else {
    printf("\ndevice name is: %s\n", name);
  }



  //  ask for software version (get HEX answer!!!)
  printf("\nsoftware version: %x\n", readSWversion() );
  

  //  RS232 timeout (14.1.35)
  if ( ( n = readRS232timeout() ) < 0) checkEPOSerror();
  else printf("\nRS232 timeout is: %d ms\n", n );
  


  // EPOS Status
  estatus = 0x0;
  if ( ( n = readStatusword(&estatus) ) < 0) checkEPOSerror();
  else printf("\nEPOS status is: %#06x \n", estatus );
  
  //printEPOSstatusword(estatus);
  //printEPOSstate();

  
  /******************************************************
            switch on  
  ******************************************************/
  
  // 1. check if we are in FAULT state

  n = checkEPOSstate();
  if ( n == 11) {
    printf("EPOS is in FAULT state, doing FAULT RESET ");
    // should check which fault this is, assume it's a CAN error from
    // power-on
       
    
    // 1a. reset FAULT
    changeEPOSstate(6);
    
    
    // 1b. check status
    if ( checkEPOSstate() == 11 ){
      fprintf(stderr, "\nEPOS still in FAULT state, quit!\n");
      exit(1);
    }
    else printf("success!\n");
  }
  else if (n != 4 && n!= 7) { // EPOS not running, issue a quick stop
    //printf("\nsending QUICK STOP\n");
    changeEPOSstate(3);
    
    // 2. we should now be in 'switch on disabled' (2)
    n = checkEPOSstate();
    if ( n != 2 ){
      printf("EPOS is NOT in 'switch on disabled' state, quit!\n");
      printf("(%s(), %s line %d)\n", __func__, __FILE__, __LINE__);
      exit(1);
    }
    else {  // EPOS is in 'switch on disabled'
      //printf("EPOS is in 'switch on disabled' state, doing shutdown. \n");
      changeEPOSstate(0); // issue a 'shutdown'
      //printEPOSstate();
    }

    // 3. switch on
    printf("\nswitching on ");
    changeEPOSstate(1);
    //printEPOSstate();

    
    // 4. enable operation
    printf("\nenable operation " );
    changeEPOSstate(5);
  }
  //printEPOSstate();



  // position window
  unsigned long win;
  readPositionWindow(&win);
  printf("\nEPOS position window is %lu. ", win);
  if (win == 4294967295 ) printf("-> position window is switched off!\n");
  else printf("\n");

  


  // actual position
  long pos = -99;
  readActualPosition(&pos);
  printf("\nEPOS position SHOULD be %ld. Starting homing... \n", pos);


    
  // filter wheel home switch is low-active; THIS IS NOT THE DEFAULT!

  if ( setHomePolarity(1) ) {
    fprintf(stderr, "\a\n *** UNABLE TO SET HomeSwitch TO low-active!!! *** \n\n");
  }
/*   if ( readDInputPolarity(&w) ) { */
/*     fprintf(stderr, "\a\n *** UNABLE TO READ Input polarity!!! *** \n\n"); */
/*   } else { */
/*     printf("input polarity is %#06x\n", w); */
/*   } */

  if ( (n = doHoming(7, -100000)) ) {
    fprintf(stderr, "\a  #### doHoming() returned %d ####\n", n);
  }
  
  readActualPosition(&pos);
  printf("\nEPOS position is %ld.\n", pos);



  

  
  /*
  n = 0;
  printf("***moving absolute to %d***\n", n);
  moveAbsolute(n);
  waitForTarget(0);


  printf("\a"); fflush(stdout);
  n = 200 *4*500; // motor-attached gear has 1:200, 1 motor axel turn
		  // has 4*500 steps (encoder gives 500counts/turn) 
  printf("\n***moving relative %d steps***\n", n);
  moveRelative(n);
  waitForTarget(0);
  printf("\a"); fflush(stdout);
  sleep(1);

  
  n *= 3;
  printf("\n***moving relative %d steps***\n", n);
  moveRelative(n);
  waitForTarget(0);
  printf("\a"); fflush(stdout);
  sleep(1);


  n = 0;
  printf("\n***and back to absolute %d***\n", n);
  moveAbsolute(n);
  monitorStatus(); // this will also wait until positoning is finished
  */


  printf("\n going to filter pos 0:  ");
  moveAbsolute(ZERO_OFFSET);
  monitorStatus(); // this will also wait until positoning is finished
  printf("  done!\a\n");
  sleep(5);

  printf("\n going to filter pos 1:  ");
  moveAbsolute(ZERO_OFFSET + 1 * FILTER_FILTER);
  monitorStatus(); // this will also wait until positoning is finished
  printf("  done!\a\n");
  sleep(5);

  printf("\n going to filter pos 3:  ");
  moveAbsolute(ZERO_OFFSET + 3 * FILTER_FILTER);
  monitorStatus(); // this will also wait until positoning is finished
  printf("  done!\a\n");
  sleep(5);
  

  printf("\n going to filter pos 5:  ");
  moveAbsolute(ZERO_OFFSET + 5 * FILTER_FILTER);
  monitorStatus(); // this will also wait until positoning is finished
  printf("  done!\a\n");












  /*****************************************/
  /*  all done, shutting off               */
  /*****************************************/
  
/*   printf("\ninitiating shutdown "); */
/*   changeEPOSstate(2); */
  printEPOSstate();
  


  closeEPOS();
  printf("\n\n");
  return 0;
}
