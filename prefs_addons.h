public:
  static void setHumanColor(int b) 
  {
      if (!self()->isImmutable( QString::fromLatin1( "HumanColor" ) ))
        self()->mHumanColor = b;
  }

  static void setComputerColor(int b) 
  {
      if (!self()->isImmutable( QString::fromLatin1( "ComputerColor" ) ))
        self()->mComputerColor= b;
  }
