void VirtualMemoryManager::swapPageIn(int virtAddr)
{
        TranslationEntry* currPageEntry;
	bool found = false;
	while(!found)
	  {
	     FrameInfo * physPageInfo = physicalMemoryInfo + nextVictim;
	    if(physPageInfo->space == NULL)
	      {
		//We assume this page is not occupied by any process space
		physPageInfo->space = currentThread->space;
		physPageInfo->pageTableIndex = virtAddr / PageSize;
		currPageEntry = getPageTableEntry(physPageInfo);
		currPageEntry->physicalPage = memoryManager->getPage();
		loadPageToCurrVictim(virtAddr);
		nextVictim = (nextVictim + 1)%NumPhysPages;
		found = true;
	      }
	    else
	      {
	        TranslationEntry* victimPageEntry = getPageTableEntry(physPageInfo);
		if(victimPageEntry->use == 0)
		  {
		    if(victimPageEntry->dirty == 1 )
		      {    //write to swap, copied from :loadPageToCurrVictim(int virtAddr)
			  int pageTableIndex = virtAddr / PageSize;
			  char* physMemLoc = machine->mainMemory + victimPageEntry->physicalPage * PageSize;
			  int swapSpaceLoc = victimPageEntry->locationOnDisk;
			  writeToSwap(physMemLoc, PageSize, swapSpaceLoc);
			   //steal	    
			  physPageInfo->space = currentThread->space;
			  physPageInfo->pageTableIndex = virtAddr / PageSize;
			  currPageEntry = getPageTableEntry(physPageInfo);
			  currPageEntry->physicalPage = victimPageEntry->physicalPage;
			  loadPageToCurrVictim(virtAddr);
			  nextVictim = (nextVictim + 1)%NumPhysPages;
			  // set their valid bit to 0
			  victimPageEntry->valid = 0;/////////////////////note this could be wrong
			  found = true;
		      }
		    else//dirty == 0
		      {
			   //steal	    
			  physPageInfo->space = currentThread->space;
			  physPageInfo->pageTableIndex = virtAddr / PageSize;
			  currPageEntry = getPageTableEntry(physPageInfo);
			  currPageEntry->physicalPage = victimPageEntry->physicalPage;
			  loadPageToCurrVictim(virtAddr);
			  nextVictim = (nextVictim + 1)%NumPhysPages;
			  // set their valid bit to 0
			  victimPageEntry->valid = 0;/////////////////////note this could be wrong
			  found = true;
		      }
		  }
		else//victimPageEntry->used ==1
		  {
		    victimPageEntry->use = 0;
		    nextVictim = (nextVictim + 1)%NumPhysPages;
		    found =true;
		  }
	      }
	  }