//*----------------------------------------------------------------------------
//*      ATMEL Microcontroller Software Support  -  ROUSSET  -
//*----------------------------------------------------------------------------
//* The software is delivered "AS IS" without warranty or condition of any
//* kind, either express, implied or statutory. This includes without
//* limitation any warranty or condition with respect to merchantability or
//* fitness for any particular purpose, or against the infringements of
//* intellectual property rights of others.
//*----------------------------------------------------------------------------
//* File Name           : obj_svctempo.c
//* Object              : service tempo
//*
//* 1.0 03/10/01 HI	    : Creation
//*----------------------------------------------------------------------------
#include "periph/system_timer/lib_st.h"
#include "services/obj_svctempo.h"

//* ====================================================================================


//* ====================================================================================


//*----------------------------------------------------------------------------
//* Function Name       : AT91F_OpenCtlTempo
//* Object              :
//* Input Parameters    :
//* Output Parameters   : none
//*----------------------------------------------------------------------------
AT91S_TempoStatus AT91F_OpenCtlTempo( AT91PS_CtlTempo pCtlTempo, void const *pTempoTimer )
{
#ifdef DBG_DRV_TIMER
	assert(pCtlTempo);
#endif

//* Check arguments of OpenSvcTempo
	if (pCtlTempo == (AT91PS_CtlTempo)0)
		return AT91C_ERROR_OPEN_CTRLTEMPO;

//* Initialize CtlTempo fields
	pCtlTempo->pFirstTempo = (AT91PS_SvcTempo)0;
	pCtlTempo->pNewTempo =  (AT91PS_SvcTempo)0;

//* default timer = system timer
	pCtlTempo->pTimer  = (void *)pTempoTimer;

	pCtlTempo->CtlTempoStart = AT91F_STStart;
	pCtlTempo->CtlTempoStop  = AT91F_STStop;


	pCtlTempo->CtlTempoSetTime = AT91F_STSetTime;
	pCtlTempo->CtlTempoGetTime = AT91F_STGetTime;
	pCtlTempo->CtlTempoIsStart = AT91F_STIsStart;

	pCtlTempo->SvcTempoStart  = AT91F_SvcTempoStart;
	pCtlTempo->SvcTempoStop   = AT91F_SvcTempoStop;
	pCtlTempo->CtlTempoCreate = AT91F_CtlTempoCreate;
	pCtlTempo->CtlTempoRemove = AT91F_CtlTempoRemove;
	pCtlTempo->CtlTempoTick   = AT91F_CtlTempoTick;

	pCtlTempo->pPrivateData = (void *) pCtlTempo;
	return AT91C_OPEN_CTRLTEMPO_SUCCESS;
}


//*----------------------------------------------------------------------------
//* Function Name       : AT91F_STStart
//* Object              : Start the systimer
//* Input Parameters    : void * pTimer
//* Output Parameters   : none
//*----------------------------------------------------------------------------
AT91S_TempoStatus AT91F_STStart(void * pTimer)
{
	AT91PS_STDesc pSysTimer = (AT91PS_STDesc)pTimer;
	AT91_STinit(pSysTimer, AT91_GetSTperiod(1000));
	return AT91C_START_OK;
}


//*----------------------------------------------------------------------------
//* Function Name       : AT91F_STStop
//* Object              :
//* Input Parameters    : void * pTimer
//* Output Parameters   : none
//*----------------------------------------------------------------------------
AT91S_TempoStatus AT91F_STStop(void * pTimer)
{
	return AT91C_STOP_OK;
}


//*----------------------------------------------------------------------------
//* Function Name       : AT91F_STSetTime
//* Object              :
//* Input Parameters    :
//* Output Parameters   : none
//*----------------------------------------------------------------------------
AT91S_TempoStatus AT91F_STSetTime(AT91PS_CtlTempo pCtrl, unsigned int NewTime)
{
	return 1;
}


//*----------------------------------------------------------------------------
//* Function Name       : AT91F_STGetTime
//* Object              :
//* Input Parameters    :
//* Output Parameters   : none
//*----------------------------------------------------------------------------
AT91S_TempoStatus AT91F_STGetTime(AT91PS_CtlTempo pCtrl)
{
	return 1;
}


//*----------------------------------------------------------------------------
//* Function Name       : AT91F_STIsStart
//* Object              :
//* Input Parameters    :
//* Output Parameters   : none
//*----------------------------------------------------------------------------
AT91S_TempoStatus AT91F_STIsStart(AT91PS_CtlTempo pCtrl)
{
	AT91PS_STDesc pSysTimer = (AT91PS_STDesc )pCtrl->pTimer;
	return(AT91F_StGetState(pSysTimer->st_base));
}


//*----------------------------------------------------------------------------
//* Function Name       : AT91F_CtlTempoCreate
//* Object              :
//* Input Parameters    :
//* Output Parameters   : none
//*----------------------------------------------------------------------------
AT91S_TempoStatus AT91F_CtlTempoCreate (
					AT91PS_CtlTempo pCtrl,
					AT91PS_SvcTempo pTempo)
{
	AT91PS_CtlTempo pCtlTempo = (AT91PS_CtlTempo)(pCtrl->pPrivateData);
	AT91PS_SvcTempo pTmp;


	//* If no tempo is activated, then insert it
	// * Check that this Tempo is not already in the current list
	pTmp = pCtlTempo->pFirstTempo;
	while (pTmp) {
		if (pTmp == pTempo)
			return 0;
		pTmp = pTmp->pNextTempo;
	}

	//* Check that this Tempo is not already in the new list
	pTmp = pCtlTempo->pNewTempo;
	while (pTmp) {
		if (pTmp == pTempo)
			return 0;
		pTmp = pTmp->pNextTempo;
	}

	//* Initialize Tempo structure
	pTempo->flag = AT91E_SVCTEMPO_DIS;
	pTempo->pPreviousTempo = (AT91PS_SvcTempo) 0;
	pTempo->Start = pCtlTempo->SvcTempoStart;
	pTempo->Stop  = pCtlTempo->SvcTempoStop;
	pTempo->TickTempo   = 0;
	pTempo->ReloadTempo = 0;

	//* Insert Tempo in the list
	if ( !(pCtlTempo->pFirstTempo) ) {
		pTempo->pNextTempo = (AT91PS_SvcTempo) 0;
		pCtlTempo->pFirstTempo = pTempo;
	}
	else {
		if (pCtlTempo->pNewTempo)
			pCtlTempo->pNewTempo->pPreviousTempo = pTempo;
		pTempo->pNextTempo = pCtrl->pNewTempo;
		pCtlTempo->pNewTempo = pTempo;
	}

	return 1;
}



//*----------------------------------------------------------------------------
//* Function Name       : AT91F_CtlTempoRemove
//* Object              :
//* Input Parameters    :
//* Output Parameters   : none
//*----------------------------------------------------------------------------
AT91S_TempoStatus AT91F_CtlTempoRemove (AT91PS_CtlTempo pCtrl, AT91PS_SvcTempo pTempo)
{
	AT91PS_CtlTempo pCtlTempo = (AT91PS_CtlTempo)(pCtrl->pPrivateData);
	AT91PS_SvcTempo pCurrentTempo = pCtlTempo->pFirstTempo;	// to go throught the tempo list

	//* Remove first Tempo entry in the list
	if(pCurrentTempo == pTempo) {
		pCtlTempo->pFirstTempo = pCurrentTempo->pNextTempo;
		pCtlTempo->pFirstTempo->pPreviousTempo = (AT91PS_SvcTempo)0;
		return 1;
	}

	while( (pCurrentTempo->pNextTempo != pTempo) && (pCurrentTempo->pNextTempo != (AT91PS_SvcTempo)0 ) )
			pCurrentTempo = (pCurrentTempo->pNextTempo);

	//* No tempo to remove
	if(	pCurrentTempo->pNextTempo == (AT91PS_SvcTempo)0 )
		return 0;
	//* Remove the selected tempo and rebuild the list
	pCurrentTempo->pNextTempo = pCurrentTempo->pNextTempo->pNextTempo;
	pCurrentTempo->pNextTempo->pPreviousTempo = pCurrentTempo;
	pCtlTempo->pPrivateData = pCtlTempo;
	return 1;
}


//*----------------------------------------------------------------------------
//* Function Name       : AT91F_CtlTempoTick
//* Object              :
//* Input Parameters    :
//* Output Parameters   : none
//*----------------------------------------------------------------------------
AT91S_TempoStatus AT91F_CtlTempoTick (AT91PS_CtlTempo pCtrl)
{
	AT91PS_CtlTempo pCtlTempo = (AT91PS_CtlTempo)(pCtrl->pPrivateData);
	AT91PS_SvcTempo pCurrentTempo;	// to go throught the tempo list

	//* Add NewTempo in FirstTempo list?
	if(pCtrl->pNewTempo != (AT91PS_SvcTempo)0 ) {
		pCurrentTempo = pCtrl->pNewTempo;
		while (pCurrentTempo->pNextTempo)
			pCurrentTempo = pCurrentTempo->pNextTempo;
		pCurrentTempo->pNextTempo = pCtlTempo->pFirstTempo;
		pCtlTempo->pFirstTempo->pPreviousTempo = pCurrentTempo;

		pCtlTempo->pFirstTempo = pCtrl->pNewTempo;
	    //* Remove NewTempo list
		pCtrl->pNewTempo = (AT91PS_SvcTempo)0;
	}

	//* Notify all the Tempo elements and executes callbacks
	pCurrentTempo = pCtlTempo->pFirstTempo;
	while (pCurrentTempo!=(AT91PS_SvcTempo)0) {
		if (pCurrentTempo->flag == AT91E_SVCTEMPO_EN) {
			if(pCurrentTempo->TickTempo > 0)
				pCurrentTempo->TickTempo--;
			if((pCurrentTempo->TickTempo  == 0) && (pCurrentTempo->TempoCallback)) {
				if (pCurrentTempo->ReloadTempo)
					pCurrentTempo->TickTempo = pCurrentTempo->ReloadTempo;
				else
					pCurrentTempo->flag = AT91E_SVCTEMPO_DIS;
				pCurrentTempo->TempoCallback(AT91C_TIMEOUT_REACHED,pCurrentTempo->pPrivateData);
			}
		}
		pCurrentTempo = pCurrentTempo->pNextTempo;
	}

	return 1;
}

//*----------------------------------------------------------------------------
//* Function Name       : AT91F_SvcTempoStart
//* Object              :
//* Input Parameters    :
//* Output Parameters   : none
//*----------------------------------------------------------------------------
AT91S_TempoStatus AT91F_SvcTempoStart (
		AT91PS_SvcTempo pSvc,
		unsigned int timeout,
		unsigned int reload,
		void (*callback) (AT91S_TempoStatus, void *),
		void *pData)
{
	pSvc->TickTempo   = timeout;
	pSvc->ReloadTempo = reload;
	pSvc->TempoCallback = callback;
	pSvc->pPrivateData = pData;
	pSvc->flag = AT91E_SVCTEMPO_EN;
	return 1;
}


//*----------------------------------------------------------------------------
//* Function Name       : AT91F_SvcTempoStop
//* Object              :
//* Input Parameters    :
//* Output Parameters   : none
//*----------------------------------------------------------------------------
AT91S_TempoStatus AT91F_SvcTempoStop (
	AT91PS_SvcTempo pSvc)
{
	pSvc->flag = AT91E_SVCTEMPO_DIS;
	return 1;
}
