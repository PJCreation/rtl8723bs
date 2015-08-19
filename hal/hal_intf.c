/******************************************************************************
 *
 * Copyright(c) 2007 - 2012 Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 *
 ******************************************************************************/

#define _HAL_INTF_C_

#include <drv_types.h>
#include <hal_data.h>

void rtw_hal_chip_configure(_adapter *padapter)
{
	if (padapter->HalFunc.intf_chip_configure)
		padapter->HalFunc.intf_chip_configure(padapter);
}

void rtw_hal_read_chip_info(_adapter *padapter)
{
	if (padapter->HalFunc.read_adapter_info)
		padapter->HalFunc.read_adapter_info(padapter);
}

void rtw_hal_read_chip_version(_adapter *padapter)
{
	if (padapter->HalFunc.read_chip_version)
		padapter->HalFunc.read_chip_version(padapter);
}

void rtw_hal_def_value_init(_adapter *padapter)
{
	if (is_primary_adapter(padapter))
		if (padapter->HalFunc.init_default_value)
			padapter->HalFunc.init_default_value(padapter);
}
void	rtw_hal_free_data(_adapter *padapter)
{
	//free HAL Data
	rtw_hal_data_deinit(padapter);

	if (is_primary_adapter(padapter))
		if (padapter->HalFunc.free_hal_data)
			padapter->HalFunc.free_hal_data(padapter);
}
void	rtw_hal_dm_init(_adapter *padapter)
{
	if (is_primary_adapter(padapter))
		if (padapter->HalFunc.dm_init)
			padapter->HalFunc.dm_init(padapter);
}
void rtw_hal_dm_deinit(_adapter *padapter)
{
	// cancel dm  timer
	if (is_primary_adapter(padapter))
		if (padapter->HalFunc.dm_deinit)
			padapter->HalFunc.dm_deinit(padapter);
}

static void rtw_hal_init_opmode(_adapter *padapter)
{
	NDIS_802_11_NETWORK_INFRASTRUCTURE networkType = Ndis802_11InfrastructureMax;
	struct  mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	sint fw_state;

	fw_state = get_fwstate(pmlmepriv);

	if (fw_state & WIFI_ADHOC_STATE)
		networkType = Ndis802_11IBSS;
	else if (fw_state & WIFI_STATION_STATE)
		networkType = Ndis802_11Infrastructure;
	else if (fw_state & WIFI_AP_STATE)
		networkType = Ndis802_11APMode;
	else
		return;

	rtw_setopmode_cmd(padapter, networkType, false);
}

uint	 rtw_hal_init(_adapter *padapter)
{
	uint	status = _SUCCESS;
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	int i;

	status = padapter->HalFunc.hal_init(padapter);

	if (status == _SUCCESS){

		rtw_hal_init_opmode(padapter);

		for (i = 0; i<dvobj->iface_nums; i++)
			dvobj->padapters[i]->hw_init_completed = true;

		if (padapter->registrypriv.notch_filter == 1)
			rtw_hal_notch_filter(padapter, 1);

		rtw_hal_reset_security_engine(padapter);

		for (i = 0; i<dvobj->iface_nums; i++)
			rtw_sec_restore_wep_key(dvobj->padapters[i]);

		init_hw_mlme_ext(padapter);

		rtw_bb_rf_gain_offset(padapter);
	}
	else{
		for (i = 0; i<dvobj->iface_nums; i++)
			dvobj->padapters[i]->hw_init_completed = false;
		DBG_871X("rtw_hal_init: hal__init fail\n");
	}

	RT_TRACE(_module_hal_init_c_,_drv_err_,("-rtl871x_hal_init:status=0x%x\n",status));

	return status;

}

uint rtw_hal_deinit(_adapter *padapter)
{
	uint	status = _SUCCESS;
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	int i;

	status = padapter->HalFunc.hal_deinit(padapter);

	if (status == _SUCCESS){
		for (i = 0; i<dvobj->iface_nums; i++) {
			padapter = dvobj->padapters[i];
			padapter->hw_init_completed = false;
		}
	}
	else
	{
		DBG_871X("\n rtw_hal_deinit: hal_init fail\n");
	}
	return status;
}

void rtw_hal_set_hwreg(_adapter *padapter, u8 variable, u8 *val)
{
	if (padapter->HalFunc.SetHwRegHandler)
		padapter->HalFunc.SetHwRegHandler(padapter, variable, val);
}

void rtw_hal_get_hwreg(_adapter *padapter, u8 variable, u8 *val)
{
	if (padapter->HalFunc.GetHwRegHandler)
		padapter->HalFunc.GetHwRegHandler(padapter, variable, val);
}

void rtw_hal_set_hwreg_with_buf(_adapter *padapter, u8 variable, u8 *pbuf, int len)
{
	if (padapter->HalFunc.SetHwRegHandlerWithBuf)
		padapter->HalFunc.SetHwRegHandlerWithBuf(padapter, variable, pbuf, len);
}

u8 rtw_hal_set_def_var(_adapter *padapter, HAL_DEF_VARIABLE eVariable, void * pValue)
{
	if (padapter->HalFunc.SetHalDefVarHandler)
		return padapter->HalFunc.SetHalDefVarHandler(padapter,eVariable,pValue);
	return _FAIL;
}
u8 rtw_hal_get_def_var(_adapter *padapter, HAL_DEF_VARIABLE eVariable, void * pValue)
{
	if (padapter->HalFunc.GetHalDefVarHandler)
		return padapter->HalFunc.GetHalDefVarHandler(padapter,eVariable,pValue);
	return _FAIL;
}

void rtw_hal_set_odm_var(_adapter *padapter, HAL_ODM_VARIABLE eVariable, void * pValue1,bool bSet)
{
	if (padapter->HalFunc.SetHalODMVarHandler)
		padapter->HalFunc.SetHalODMVarHandler(padapter,eVariable,pValue1,bSet);
}
void	rtw_hal_get_odm_var(_adapter *padapter, HAL_ODM_VARIABLE eVariable, void * pValue1,void * pValue2)
{
	if (padapter->HalFunc.GetHalODMVarHandler)
		padapter->HalFunc.GetHalODMVarHandler(padapter,eVariable,pValue1,pValue2);
}

void rtw_hal_enable_interrupt(_adapter *padapter)
{
	if (padapter->HalFunc.enable_interrupt)
		padapter->HalFunc.enable_interrupt(padapter);
	else
		DBG_871X("%s: HalFunc.enable_interrupt is NULL!\n", __FUNCTION__);

}
void rtw_hal_disable_interrupt(_adapter *padapter)
{
	if (padapter->HalFunc.disable_interrupt)
		padapter->HalFunc.disable_interrupt(padapter);
	else
		DBG_871X("%s: HalFunc.disable_interrupt is NULL!\n", __FUNCTION__);

}

u8 rtw_hal_check_ips_status(_adapter *padapter)
{
	u8 val = false;
	if (padapter->HalFunc.check_ips_status)
		val = padapter->HalFunc.check_ips_status(padapter);
	else
		DBG_871X("%s: HalFunc.check_ips_status is NULL!\n", __FUNCTION__);

	return val;
}

#if defined(CONFIG_WOWLAN) || defined(CONFIG_AP_WOWLAN)
void rtw_hal_clear_interrupt(_adapter *padapter)
{
    if (padapter->HalFunc.clear_interrupt)
        padapter->HalFunc.clear_interrupt(padapter);
    else
        DBG_871X("%s: HalFunc.clear_interrupt is NULL!\n", __FUNCTION__);

}
#endif

s32	rtw_hal_xmitframe_enqueue(_adapter *padapter, struct xmit_frame *pxmitframe)
{
	if (padapter->HalFunc.hal_xmitframe_enqueue)
		return padapter->HalFunc.hal_xmitframe_enqueue(padapter, pxmitframe);

	return false;
}

s32	rtw_hal_xmit(_adapter *padapter, struct xmit_frame *pxmitframe)
{
	if (padapter->HalFunc.hal_xmit)
		return padapter->HalFunc.hal_xmit(padapter, pxmitframe);

	return false;
}

/*
 * [IMPORTANT] This function would be run in interrupt context.
 */
s32	rtw_hal_mgnt_xmit(_adapter *padapter, struct xmit_frame *pmgntframe)
{
	s32 ret = _FAIL;
	update_mgntframe_attrib_addr(padapter, pmgntframe);
	//pframe = (u8 *)(pmgntframe->buf_addr) + TXDESC_OFFSET;
	//pwlanhdr = (struct rtw_ieee80211_hdr *)pframe;
	//memcpy(pmgntframe->attrib.ra, pwlanhdr->addr1, ETH_ALEN);

	if (padapter->securitypriv.binstallBIPkey == true)
	{
		if (IS_MCAST(pmgntframe->attrib.ra))
		{
			pmgntframe->attrib.encrypt = _BIP_;
			//pmgntframe->attrib.bswenc = true;
		}
		else
		{
			pmgntframe->attrib.encrypt = _AES_;
			pmgntframe->attrib.bswenc = true;
		}
		rtw_mgmt_xmitframe_coalesce(padapter, pmgntframe->pkt, pmgntframe);
	}

	if (padapter->HalFunc.mgnt_xmit)
		ret = padapter->HalFunc.mgnt_xmit(padapter, pmgntframe);
	return ret;
}

s32	rtw_hal_init_xmit_priv(_adapter *padapter)
{
	if (padapter->HalFunc.init_xmit_priv != NULL)
		return padapter->HalFunc.init_xmit_priv(padapter);
	return _FAIL;
}
void	rtw_hal_free_xmit_priv(_adapter *padapter)
{
	if (padapter->HalFunc.free_xmit_priv != NULL)
		padapter->HalFunc.free_xmit_priv(padapter);
}

s32	rtw_hal_init_recv_priv(_adapter *padapter)
{
	if (padapter->HalFunc.init_recv_priv)
		return padapter->HalFunc.init_recv_priv(padapter);

	return _FAIL;
}
void	rtw_hal_free_recv_priv(_adapter *padapter)
{

	if (padapter->HalFunc.free_recv_priv)
		padapter->HalFunc.free_recv_priv(padapter);
}

void rtw_hal_update_ra_mask(struct sta_info *psta, u8 rssi_level)
{
	_adapter *padapter;
	struct mlme_priv *pmlmepriv;

	if (!psta)
		return;

	padapter = psta->padapter;

	pmlmepriv = &(padapter->mlmepriv);

	if (check_fwstate(pmlmepriv, WIFI_AP_STATE) == true)
	{
		add_RATid(padapter, psta, rssi_level);
	}
	else
	{
		if (padapter->HalFunc.UpdateRAMaskHandler)
			padapter->HalFunc.UpdateRAMaskHandler(padapter, psta->mac_id, rssi_level);
	}
}

void	rtw_hal_add_ra_tid(_adapter *padapter, u32 bitmap, u8* arg, u8 rssi_level)
{
	if (padapter->HalFunc.Add_RateATid)
		padapter->HalFunc.Add_RateATid(padapter, bitmap, arg, rssi_level);
}

/*	Start specifical interface thread		*/
void	rtw_hal_start_thread(_adapter *padapter)
{
	if (padapter->HalFunc.run_thread)
		padapter->HalFunc.run_thread(padapter);
}
/*	Start specifical interface thread		*/
void	rtw_hal_stop_thread(_adapter *padapter)
{
	if (padapter->HalFunc.cancel_thread)
		padapter->HalFunc.cancel_thread(padapter);
}

u32	rtw_hal_read_bbreg(_adapter *padapter, u32 RegAddr, u32 BitMask)
{
	u32 data = 0;
	if (padapter->HalFunc.read_bbreg)
		 data = padapter->HalFunc.read_bbreg(padapter, RegAddr, BitMask);
	return data;
}
void	rtw_hal_write_bbreg(_adapter *padapter, u32 RegAddr, u32 BitMask, u32 Data)
{
	if (padapter->HalFunc.write_bbreg)
		padapter->HalFunc.write_bbreg(padapter, RegAddr, BitMask, Data);
}

u32	rtw_hal_read_rfreg(_adapter *padapter, u32 eRFPath, u32 RegAddr, u32 BitMask)
{
	u32 data = 0;
	if ( padapter->HalFunc.read_rfreg)
		data = padapter->HalFunc.read_rfreg(padapter, eRFPath, RegAddr, BitMask);
	return data;
}
void	rtw_hal_write_rfreg(_adapter *padapter, u32 eRFPath, u32 RegAddr, u32 BitMask, u32 Data)
{
	if (padapter->HalFunc.write_rfreg)
		padapter->HalFunc.write_rfreg(padapter, eRFPath, RegAddr, BitMask, Data);
}

void	rtw_hal_set_chan(_adapter *padapter, u8 channel)
{
	if (padapter->HalFunc.set_channel_handler)
		padapter->HalFunc.set_channel_handler(padapter, channel);
}

void	rtw_hal_set_chnl_bw(_adapter *padapter, u8 channel, CHANNEL_WIDTH Bandwidth, u8 Offset40, u8 Offset80)
{
	if (padapter->HalFunc.set_chnl_bw_handler)
		padapter->HalFunc.set_chnl_bw_handler(padapter, channel, Bandwidth, Offset40, Offset80);
}

void	rtw_hal_dm_watchdog(_adapter *padapter)
{
	if (padapter->HalFunc.hal_dm_watchdog)
		padapter->HalFunc.hal_dm_watchdog(padapter);

}

void	rtw_hal_dm_watchdog_in_lps(_adapter *padapter)
{
	if (adapter_to_pwrctl(padapter)->bFwCurrentInPSMode ==true )
	{
		if (padapter->HalFunc.hal_dm_watchdog_in_lps)
		{
			padapter->HalFunc.hal_dm_watchdog_in_lps(padapter);//this fuction caller is in interrupt context
		}
	}
}

void rtw_hal_bcn_related_reg_setting(_adapter *padapter)
{
	if (padapter->HalFunc.SetBeaconRelatedRegistersHandler)
		padapter->HalFunc.SetBeaconRelatedRegistersHandler(padapter);
}


s32 rtw_hal_xmit_thread_handler(_adapter *padapter)
{
	if (padapter->HalFunc.xmit_thread_handler)
		return padapter->HalFunc.xmit_thread_handler(padapter);
	return _FAIL;
}

void rtw_hal_notch_filter(_adapter *adapter, bool enable)
{
	if (adapter->HalFunc.hal_notch_filter)
		adapter->HalFunc.hal_notch_filter(adapter,enable);
}

void rtw_hal_reset_security_engine(_adapter * adapter)
{
	if (adapter->HalFunc.hal_reset_security_engine)
		adapter->HalFunc.hal_reset_security_engine(adapter);
}

bool rtw_hal_c2h_valid(_adapter *adapter, u8 *buf)
{
	return c2h_evt_valid((struct c2h_evt_hdr_88xx*)buf);
}

s32 rtw_hal_c2h_evt_read(_adapter *adapter, u8 *buf)
{
	return c2h_evt_read_88xx(adapter, buf);
}

s32 rtw_hal_c2h_handler(_adapter *adapter, u8 *c2h_evt)
{
	s32 ret = _FAIL;
	if (adapter->HalFunc.c2h_handler)
		ret = adapter->HalFunc.c2h_handler(adapter, c2h_evt);
	return ret;
}

c2h_id_filter rtw_hal_c2h_id_filter_ccx(_adapter *adapter)
{
	return adapter->HalFunc.c2h_id_filter_ccx;
}

s32 rtw_hal_is_disable_sw_channel_plan(PADAPTER padapter)
{
	return GET_HAL_DATA(padapter)->bDisableSWChannelPlan;
}

s32 rtw_hal_macid_sleep(PADAPTER padapter, u32 macid)
{
	u8 support;


	support = false;
	rtw_hal_get_def_var(padapter, HAL_DEF_MACID_SLEEP, &support);
	if (false == support)
		return _FAIL;

	rtw_hal_set_hwreg(padapter, HW_VAR_MACID_SLEEP, (u8*)&macid);

	return _SUCCESS;
}

s32 rtw_hal_macid_wakeup(PADAPTER padapter, u32 macid)
{
	u8 support;


	support = false;
	rtw_hal_get_def_var(padapter, HAL_DEF_MACID_SLEEP, &support);
	if (false == support)
		return _FAIL;

	rtw_hal_set_hwreg(padapter, HW_VAR_MACID_WAKEUP, (u8*)&macid);

	return _SUCCESS;
}

s32 rtw_hal_fill_h2c_cmd(PADAPTER padapter, u8 ElementID, u32 CmdLen, u8 *pCmdBuffer)
{
	s32 ret = _FAIL;

	if (padapter->HalFunc.fill_h2c_cmd)
		ret = padapter->HalFunc.fill_h2c_cmd(padapter, ElementID, CmdLen, pCmdBuffer);
	else
	{
		DBG_871X("%s:  func[fill_h2c_cmd] not defined!\n", __FUNCTION__);
	}

	return ret;
}
