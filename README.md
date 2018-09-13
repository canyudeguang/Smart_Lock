SmartLock
=====================================================================
  This project is based on Qualcomm QCA4020 development board whose name is Smart Lock. The main objective of this demo is done
  by using an application on mobile phone to connect to the QCA4020 via BLE(Bluetooth Low Energy),and there will PWM wave output
  through GPIO of QCA4020,at the same time,servo motor will have angle change at different PWM situations.Actually the servo motor
  of this demo is designed to simulate a smart lock.  


Instruction:
----------------------

Hardware requirements
--------------
  QCA4020 development board, Servo motor  

Software requirements
--------  
1)toolchain
--
    The toochain can be downloaded from the ARM website at:  
        https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads   
    Add the path to toolchain binaries to 'PATH' environment variable.  
	  
2)Python
--
    Some of the support scripts are Python-based, Python 2.7.2, or higher. You can download the Python official website.
    After installation, add the path to python to 'PATH' environment variable.  

Assembly Instructions
-----------------------------------------------------------------------------
    According to the above table, install the jumper of QCA4020 board. Note that J3 and J1 are for servo motor connecting
	to QCA4020 board.  

Usage Instructions
--------------------------
(1)Download code from github according to this repositary.  
(2)Compile the code and flash the image according to "80-ya121-140-qca402xcdb2x_development_kit_user_guide" file.  
(3)Power on the QCA4020  
(4)Open the app named "LightBlue" on iPhone, and "LightBlue" will search BLE automatically, then connect the spot named "lpw-spple-demo".  
(5)The service we used is called "UUID:1815", and we use the fourth characteristic to control motor.  
(6)We input "FFFF", the lock will be "open", and "0000" the lock will be "closed".  
