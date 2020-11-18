import { useEffect, useState } from "preact/hooks";

import { useFetch, useDelete } from "../../api";
import { processRedirect, logError } from "./../../utils";
import { ENTRY_TYPE } from "./../../utils";
import { BatteryStatus } from "./../../components/battery.status";
import { Button } from "../../components/button";
import { Card } from "../../components/card";
import { CardContainer } from "../../components/card.container";
import { IconLink } from "../../components/icon.link";
import { Statusbar } from "../../components/statusbar";
import { Typography, Variant } from "../../components/typography";

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
  const { fetch: setupState } = useFetch("/api/setup");
  const [events, setEvents] = useState([]);
  const { fetch: fetchEntries, isLoading } = useFetch("/api/events");
  const { delete: clearEntries } = useDelete("/api/events/clear");
  const [entryInterval, setEntryInterval] = useState();

  const getEntries = async () => {
    try {
      const { events } = await fetchEntries();
      setEvents(events);
    } catch (e) {
      logError(e);
      clearInterval(entryInterval);
    }
  };

  useEffect(() => {
    const setup = async () => {
      const { status } = await setupState();
      processRedirect(status);
    };
    setup();

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
        {events.length === 0 && (
          <Typography variant={Variant.h1} text="Brak powiadomień" />
        )}
        {events.map((entry) => composeCard(entry))}
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
