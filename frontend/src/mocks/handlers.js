import { rest } from "msw";

const WIFI_TYPES = {
  WIFI_AUTH_OPEN: 0,
  WIFI_AUTH_WEP: 1,
  WIFI_AUTH_WPA_PSK: 2,
  WIFI_AUTH_WPA2_PSK: 3,
  WIFI_AUTH_WPA_WPA2_PSK: 4,
  WIFI_AUTH_WPA2_ENT: 5,
  WIFI_AUTH_MAX: 6,
};

const WIFI_STATUS = {
  WL_IDLE_STATUS: 0,
  WL_NO_SSID_AVAIL: 1,
  WL_SCAN_COMPLETED: 2,
  WL_CONNECTED: 3,
  WL_CONNECT_FAILED: 4,
  WL_CONNECTION_LOST: 5,
  WL_DISCONNECTED: 6,
};

const MQTT_STATUS = {
  DISCONNECTED: 0,
  CONNECTED: 1,
  CONNECTING: 2,
};

const SETUP_STATUS = {
  NOT_COMPLETED: 0,
  WIFI_COMPLETED: 1,
  COMPLETED: 2,
};

const ENTRY_TYPE = {
  POWER_LOSS: 0,
  POWER_ON: 1,
  BATTERY_LOW: 2,
};

const ENTRIES = [
  {
    type: ENTRY_TYPE.POWER_LOSS,
    date: "2020-07-12 13:30",
  },
  {
    type: ENTRY_TYPE.POWER_ON,
    date: "2020-07-12 13:32",
  },
  {
    type: ENTRY_TYPE.POWER_LOSS,
    date: "2020-07-12 13:40",
  },
  {
    type: ENTRY_TYPE.BATTERY_LOW,
    date: "2020-07-12 13:50",
  },
  {
    type: ENTRY_TYPE.POWER_LOSS,
    date: "2020-07-12 13:30",
  },
  {
    type: ENTRY_TYPE.POWER_ON,
    date: "2020-07-12 13:32",
  },
  {
    type: ENTRY_TYPE.POWER_LOSS,
    date: "2020-07-12 13:40",
  },
  {
    type: ENTRY_TYPE.BATTERY_LOW,
    date: "2020-07-12 13:50",
  },
];

export const handlers = [
  rest.get("/api/setup/config", (req, res, ctx) => {
    return res(
      ctx.status(200),
      ctx.json({
        ssid: "NETWORK SSID",
        port: 3306,
        server: "mqtt.example.com",
        user: "sip.user.mqtt",
      })
    );
  }),
  rest.post("/api/entries/clear", (req, res, ctx) => {
    sessionStorage.setItem("entries", 0);
    return res(ctx.status(201));
  }),
  rest.get("/api/entries", (req, res, ctx) => {
    let items = sessionStorage.getItem("entries");
    if (items >= ENTRIES.length) items = ENTRIES.length;
    const entries = [...ENTRIES.slice(0, items)];
    sessionStorage.setItem("entries", ++items);

    return res(
      ctx.status(200),
      ctx.json({
        entries,
      })
    );
  }),
  rest.get("/api/setup", (req, res, ctx) => {
    if (sessionStorage.getItem("isConnected")) {
      if (sessionStorage.getItem("isMqttConnected")) {
        return res(
          ctx.status(200),
          ctx.json({
            status: SETUP_STATUS.COMPLETED,
          })
        );
      }

      return res(
        ctx.status(200),
        ctx.json({
          status: SETUP_STATUS.WIFI_COMPLETED,
        })
      );
    }

    return res(
      ctx.status(200),
      ctx.json({
        status: SETUP_STATUS.NOT_COMPLETED,
      })
    );
  }),
  rest.get("/api/battery", (req, res, ctx) => {
    return res(
      ctx.status(200),
      ctx.json({
        voltage: 3700 + Math.round(Math.random() * 500),
      })
    );
  }),
  rest.post("/api/mqtt/connect", (req, res, ctx) => {
    sessionStorage.removeItem("isMqttConnecting");
    sessionStorage.removeItem("isMqttConnected");
    if (req.body.server === "mqtt.example.com") {
      sessionStorage.setItem("isMqttConnecting", true);
    }
    return res(ctx.status(201));
  }),
  rest.get("/api/mqtt/status", (req, res, ctx) => {
    if (sessionStorage.getItem("isMqttConnected")) {
      return res(
        ctx.status(200),
        ctx.json({
          status: MQTT_STATUS.CONNECTED,
        })
      );
    }
    if (sessionStorage.getItem("isMqttConnecting")) {
      sessionStorage.setItem("isMqttConnected", true);
      return res(
        ctx.status(200),
        ctx.json({
          status: MQTT_STATUS.CONNECTING,
        })
      );
    }
    return res(
      ctx.status(200),
      ctx.json({
        status: MQTT_STATUS.DISCONNECTED,
      })
    );
  }),
  rest.get("/api/wifi/scan", (req, res, ctx) => {
    return res(ctx.status(201));
  }),
  rest.post("/api/wifi/connect", (req, res, ctx) => {
    sessionStorage.setItem("isConnected", true);
    return res(ctx.status(201));
  }),
  rest.get("/api/wifi/status", (req, res, ctx) => {
    if (sessionStorage.getItem("isConnected")) {
      return res(
        ctx.status(200),
        ctx.json({
          status: WIFI_STATUS.WL_CONNECTED,
        })
      );
    }

    return res(
      ctx.status(200),
      ctx.json({
        status: WIFI_STATUS.WL_DISCONNECTED,
      })
    );
  }),
  rest.get("/api/wifi/networks", (req, res, ctx) => {
    return res(
      ctx.status(200),
      ctx.json({
        networks: [
          {
            id: 0,
            ssid: "Nazwa sieci OPEN",
            rssi: -86,
            type: WIFI_TYPES.WIFI_AUTH_OPEN,
          },
          {
            id: 1,
            ssid: "Nazwa sieci WEP",
            rssi: -76,
            type: WIFI_TYPES.WIFI_AUTH_WEP,
          },
          {
            id: 2,
            ssid: "Nazwa sieci WPA",
            rssi: -72,
            type: WIFI_TYPES.WIFI_AUTH_WPA_PSK,
          },
          {
            id: 3,
            ssid: "Nazwa sieci WPA2",
            rssi: -66,
            type: WIFI_TYPES.WIFI_AUTH_WPA2_PSK,
          },
          {
            id: 4,
            ssid: "Nazwa sieci WPA(2)",
            rssi: -86,
            type: WIFI_TYPES.WIFI_AUTH_WPA_WPA2_PSK,
          },
        ],
      })
    );
  }),
];
