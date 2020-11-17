import { useEffect, useState } from "preact/hooks";

import { Icon } from "../icon";
import { useFetch } from "./../../api/";

const BATTERY = {
  FULL: "/assets/bat-full.svg",
  MEDIUM: "/assets/bat.svg",
  LOW: "/assets/bat-low.svg",
  MINIMUM_VOLTAGE: 3700,
  MAXIMUM_VOLTAGE: 4175,
};

export const BatteryStatus = () => {
  const { fetch: getStatus } = useFetch("/api/setup/battery");
  const [iconSrc, setIconSrc] = useState(BATTERY.FULL);

  const round = (value) =>
    Math.max(
      Math.min(
        Math.ceil(
          ((value - BATTERY.MINIMUM_VOLTAGE) /
            (BATTERY.MAXIMUM_VOLTAGE - BATTERY.MINIMUM_VOLTAGE)) *
            100
        ),
        100
      ),
      0
    );

  const selectIcon = (percent) => {
    if (percent > 70) return BATTERY.FULL;
    else if (percent > 40) return BATTERY.MEDIUM;
    else return BATTERY.LOW;
  };

  useEffect(() => {
    const interval = setInterval(async () => {
      const { voltage } = await getStatus();
      setIconSrc(selectIcon(round(voltage)));
    }, 5000);

    return () => {
      clearInterval(interval);
    };
  }, []);

  return <Icon src={iconSrc} />;
};

export default BatteryStatus;
