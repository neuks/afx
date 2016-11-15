############################################################################### 
# General Purpose Makefile
# 
# Copyright (C) 2010, Martin Tang
############################################################################### 

# Configurations
INCLUDE=-DDEBUG
ASFLAGS=$(INCLUDE) -O2
CCFLAGS=$(INCLUDE) -O2
CXFLAGS=$(INCLUDE) -O2
FCFLAGS=$(INCLUDE) -O2
LDFLAGS=

# Objectives
OBJECTS=afx.o
TARGETS=libafx.a
DEPENDS=$(OBJECTS:.o=.dep) 

# Build Commands
all : $(TARGETS)

$(TARGETS) : $(OBJECTS) 
	@echo [LD] $@
	@ar cr $@ $^

debug: all

run: all 

clean:  
	@echo [RM] $(OBJECTS) $(TARGETS)
	@rm $(DEPENDS) $(OBJECTS) $(TARGETS)

# Standard Procedures
%.dep : %.s 
	@cpp $(INCLUDE) -MM -MT $(@:.dep=.o) -o $@ $< 

%.dep : %.c 
	@cpp $(INCLUDE) -MM -MT $(@:.dep=.o) -o $@ $< 

%.dep : %.m 
	@cpp $(INCLUDE) -MM -MT $(@:.dep=.o) -o $@ $< 

%.dep : %.cpp 
	@cpp $(INCLUDE) -MM -MT $(@:.dep=.o) -o $@ $< 

%.dep : %.f 
	@cpp $(INCLUDE) -MM -MT $(@:.dep=.o) -o $@ $< 

%.dep : %.rc 
	@cpp $(INCLUDE) -MM -MT $(@:.dep=.o) -o $@ $< 

%.dep : %.l
	@cpp $(INCLUDE) -MM -MT $(@:.dep=.o) -o $@ $< 

%.dep : %.y
	@cpp $(INCLUDE) -MM -MT $(@:.dep=.o) -o $@ $< 

%.o : %.s 
	@echo [AS] $<
	@as $(ASFLAGS) -o $@ $< 

%.o : %.c 
	@echo [CC] $<
	@gcc $(CCFLAGS) -c -o $@ $< 

%.o : %.m 
	@echo [OC] $<
	@gcc $(CCFLAGS) -c -o $@ $< 

%.o : %.cpp 
	@echo [CX] $<
	@g++ $(CXFLAGS) -c -o $@ $< 

%.o : %.f 
	@echo [FC] $<
	@g77 $(FCFLAGS) -c -o $@ $< 

%.o : %.rc 
	@echo [RC] $<
	@windres $< $@ 

%.c : %.l
	@echo [LX] $<
	@flex -o $@ $<

%.c : %.y
	@echo [BS] $<
	@bison -d -o $@ $<

-include $(DEPENDS)
