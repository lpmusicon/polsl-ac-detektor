import { Routes } from "../components/app.routes";
import { route } from "preact-router";

const SETUP_STATUS = {
  NOT_COMPLETED: 0,
  WIFI_COMPLETED: 1,
  COMPLETED: 2,
};

export const WIFI_STATUS = {
  WL_IDLE_STATUS: 0,
  WL_NO_SSID_AVAIL: 1,
  WL_SCAN_COMPLETED: 2,
  WL_CONNECTED: 3,
  WL_CONNECT_FAILED: 4,
  WL_CONNECTION_LOST: 5,
  WL_DISCONNECTED: 6,
};

export const processRedirect = (status) => {
  console.log("Current status: ", status);
  if (status === SETUP_STATUS.NOT_COMPLETED) {
    route(Routes.SetupWifi.path, true);
  } else if (status === SETUP_STATUS.WIFI_COMPLETED) {
    route(Routes.SetupMQTT.path, true);
  } else if (status === SETUP_STATUS.COMPLETED) {
    route(Routes.Dashboard.path, true);
  }
};

export const ENTRY_TYPE = {
  POWER_LOSS: 0,
  POWER_ON: 1,
  BATTERY_LOW: 2,
};

export const MQTT_STATUS = {
  DISCONNECTED: 0,
  CONNECTED: 1,
  CONNECTING: 2,
};
