import { Field, useFormikContext } from "formik";

import { useEffect } from "preact/hooks";

import { Typography, Variant } from "../../../../components/typography";

import { EntryWrapper } from "./entry.wrapper";
import { Password } from "./password";
import { SSID } from "./ssid";

const WIFI_TYPES = {
  WIFI_AUTH_OPEN: 0,
  WIFI_AUTH_WEP: 1,
  WIFI_AUTH_WPA_PSK: 2,
  WIFI_AUTH_WPA2_PSK: 3,
  WIFI_AUTH_WPA_WPA2_PSK: 4,
  WIFI_AUTH_WPA2_ENT: 5,
  WIFI_AUTH_MAX: 6,
};

const WifiEntry = ({ network, setActive, selected }) => {
  const { setFieldValue } = useFormikContext();
  useEffect(() => {
    if (selected) {
      setFieldValue("password", "");
    }
  }, [selected]);

  const onClick = () => {
    setActive(network.id);
    setFieldValue("ssid", network.ssid);
    setFieldValue("type", network.type);
  };

  return (
    <EntryWrapper>
      <SSID onClick={onClick} active={selected}>
        <Typography variant={Variant.body2} text={network.ssid} />
        <Typography variant={Variant.accent} text={network.rssi} />
      </SSID>
      {network.type !== WIFI_TYPES.WIFI_AUTH_OPEN && selected && (
        <Field
          name="password"
          type="password"
          autocomplete="off"
          component={Password}
          maxlength="63"
          placeholder="***** ***"
        />
      )}
    </EntryWrapper>
  );
};

export default WifiEntry;
