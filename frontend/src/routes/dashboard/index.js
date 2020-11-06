import { useEffect, useState } from "preact/hooks";
import { useFetch, usePost } from "../../api";

import { BatteryStatus } from "./../../components/battery.status";
import { Button } from "../../components/button";
import { Card } from "../../components/card";
import { CardContainer } from "../../components/card.container";
import { ENTRY_TYPE } from "./../../utils";
import { IconLink } from "../../components/icon.link";
import { Statusbar } from "../../components/statusbar";

const composeCard = (entry) => {
  switch (entry.type) {
    case ENTRY_TYPE.POWER_LOSS:
      return (
        <Card icon="assets/ac.svg" title="Utrata Zasilania" date={entry.date} />
      );
    case ENTRY_TYPE.POWER_ON:
      return (
        <Card
          icon="assets/bat-charge.svg"
          title="Podłączenie zasilania"
          date={entry.date}
        />
      );
    case ENTRY_TYPE.BATTERY_LOW:
      return (
        <Card
          icon="assets/bat-low.svg"
          title="Niski poziom baterii"
          date={entry.date}
        />
      );
  }
};

export const Dashboard = () => {
  const [entries, setEntries] = useState([]);
  const { fetch: fetchEntries, isLoading } = useFetch("/api/entries");
  const { post: clearEntries } = usePost("/api/entries/clear");
  const [entryInterval, setEntryInterval] = useState();

  const getEntries = async () => {
    try {
      const { entries } = await fetchEntries();
      setEntries(entries);
    } catch (e) {
      console.warn("Error: ", e);
      clearInterval(entryInterval);
    }
  };

  useEffect(() => {
    const interval = setInterval(async () => getEntries(), 5000);
    setEntryInterval(interval);
    getEntries();

    return () => {
      clearInterval(interval);
    };
  }, []);

  return (
    <>
      <Statusbar
        title="Dashboard"
        left={<BatteryStatus />}
        right={
          <IconLink href="/settings" src="/assets/setup.svg" alt="settings" />
        }
      />
      <CardContainer>
        {entries.map((entry) => composeCard(entry))}
      </CardContainer>
      <Button
        disabled={isLoading}
        onClick={async () => {
          await clearEntries();
          await getEntries();
        }}
      >
        Wyczyść powiadomienia
      </Button>
    </>
  );
};
