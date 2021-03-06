MAINDIR	 								= src/main/
MODULESDIR 								= src/modules/
DATASETSMOD								= datasets_mod/
UTILSDIR								= src/utils/
OBSTMOD 								= obst_mod/
CONFIGMOD								= config_mod/
OUTPUTMOD								= output_mod/
LIBSDIR									= libs/
JSMNLIBS								= jsmn/
PARTITIONINGMOD							= partitioning_mod/
IRREGULARPARTITIONINGDIR				= irregular_partitioning/
IRREGULARPARTITIONINGWITHPDIR			= p/
IRREGULARPARTITIONINGWITHSQRT2PDIR		= sqrt_2p/
REGULARPARTITIONINGDIR					= regular_partitioning/
COMPIIIDBYDDIR 							= compiii_dbyd/
COMPIIIPBYPDIR 							= compiii_pbyp/
COMPIIIDSEJDIR 							= compiii_dsej/
COMPIII4SDIR 							= compiii_4s/
KECHIDDPBADIR 							= kechid_dpba/

DATASETSMODDIR 							= $(MODULESDIR)$(DATASETSMOD)
OBSTMODDIR								= $(MODULESDIR)$(OBSTMOD)
CONFIGMODDIR							= $(MODULESDIR)$(CONFIGMOD)
JSMNLIBSDIR								= $(LIBSDIR)$(JSMNLIBS)
PARTITIONINGMODDIR						= $(MODULESDIR)$(PARTITIONINGMOD)
IRREGULARPARTITIONINGMODDIR				= $(MODULESDIR)$(PARTITIONINGMOD)$(IRREGULARPARTITIONINGDIR)
IRREGULARPARTITIONINGWITHPMODDIR		= $(IRREGULARPARTITIONINGMODDIR)$(IRREGULARPARTITIONINGWITHPDIR)
IRREGULARPARTITIONINGWITHSQRT2PMODDIR	= $(IRREGULARPARTITIONINGMODDIR)$(IRREGULARPARTITIONINGWITHSQRT2PDIR)
REGULARPARTITIONINGMODDIR				= $(MODULESDIR)$(PARTITIONINGMOD)$(REGULARPARTITIONINGDIR)
OUTPUTMODDIR							= $(MODULESDIR)$(OUTPUTMOD)
COMPIIIDBYDMODDIR 						= $(OBSTMODDIR)$(COMPIIIDBYDDIR)
COMPIIIPBYPMODDIR 						= $(OBSTMODDIR)$(COMPIIIPBYPDIR)
COMPIIIDSEJMODDIR 						= $(OBSTMODDIR)$(COMPIIIDSEJDIR)
COMPIII4SMODDIR 						= $(OBSTMODDIR)$(COMPIII4SDIR)
KECHIDDPBAMODDIR 						= $(OBSTMODDIR)$(KECHIDDPBADIR)

MAINOBJ									= $(MAINDIR)main.c
DATASETSOBJ								= $(DATASETSMODDIR)datasets.c
UTILSOBJ								= $(UTILSDIR)utils.c
OBSTOBJ									= $(OBSTMODDIR)obst.c
JSMNOBJ									= $(JSMNLIBSDIR)jsmn.c
CONFIGOBJ								= $(CONFIGMODDIR)config.c
CLOGGEROBJ								= $(UTILSDIR)clogger.c
PARTITIONINGOBJ							= $(PARTITIONINGMODDIR)partitioning.c
IRREGULARPARTITIONINGOBJ				= $(IRREGULARPARTITIONINGMODDIR)irregularPartitioning.c
IRREGULARPARTITIONINGWITHPOBJ			= $(IRREGULARPARTITIONINGWITHPMODDIR)irregularPartitioningWithP.c
IRREGULARPARTITIONINGWITHSQRT2POBJ		= $(IRREGULARPARTITIONINGWITHSQRT2PMODDIR)irregularPartitioningWithSqrt2P.c
REGULARPARTITIONINGOBJ					= $(REGULARPARTITIONINGMODDIR)regularPartitioning.c
OUTPUTOBJ								= $(OUTPUTMODDIR)output.c
COMPIIIDBYDOBJ							= $(COMPIIIDBYDMODDIR)compiii_dbyd.c
COMPIIIPBYPOBJ							= $(COMPIIIPBYPMODDIR)compiii_pbyp.c
COMPIIIDSEJOBJ							= $(COMPIIIDSEJMODDIR)compiii_dsej.c
COMPIII4SOBJ							= $(COMPIII4SMODDIR)compiii_4s.c
KECHIDDPBAOBJ							= $(KECHIDDPBAMODDIR)kechid_dpba.c

CC 										= mpicc
CFLAGS   								= -Wall -g
LDFLAGS  								= -lm -fcommon
IDFLAGS	 								= -I$(MAINDIR) -I$(DATASETSMODDIR) -I$(OBSTMODDIR) -I$(CONFIGMODDIR) -I$(UTILSDIR) -I$(JSMNLIBSDIR) -I$(OUTPUTMODDIR) -I$(PARTITIONINGMODDIR) -I$(REGULARPARTITIONINGMODDIR) -I$(IRREGULARPARTITIONINGMODDIR) -I$(IRREGULARPARTITIONINGWITHPMODDIR) -I$(IRREGULARPARTITIONINGWITHSQRT2PMODDIR) -I$(COMPIIIDBYDMODDIR) -I$(COMPIIIPBYPMODDIR) -I$(COMPIIIDSEJMODDIR) -I$(COMPIII4SMODDIR) -I$(KECHIDDPBAMODDIR)
OBJFILES 								= $(MAINOBJ) $(DATASETSOBJ) $(OBSTOBJ) $(UTILSOBJ) $(CLOGGEROBJ) $(JSMNOBJ) $(CONFIGOBJ) $(OUTPUTOBJ) $(PARTITIONINGOBJ) $(REGULARPARTITIONINGOBJ) $(IRREGULARPARTITIONINGOBJ) $(IRREGULARPARTITIONINGWITHPOBJ) $(IRREGULARPARTITIONINGWITHSQRT2POBJ) $(COMPIIIDBYDOBJ) $(COMPIIIPBYPOBJ) $(COMPIIIDSEJOBJ) $(COMPIII4SOBJ) $(KECHIDDPBAOBJ)
EXEC   									= bin/CGM-OBST.run

all: $(EXEC)
$(EXEC):
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJFILES) $(IDFLAGS) $(LDFLAGS)

clean:
	@rm -f $(EXEC)
mrproper: clean
	@rm -f $(EXEC)