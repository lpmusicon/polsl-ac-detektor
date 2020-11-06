import axios from "axios";
import { useState } from "preact/hooks";

export const useFetch = (url) => {
  const [isLoading, setIsLoading] = useState(false);

  const fetch = async () => {
    setIsLoading(true);
    let response;
    try {
      response = await axios.get(url);
    } catch (e) {
      throw e;
    } finally {
      setIsLoading(false);
    }

    return {
      ...response.data,
      statusCode: response.status,
    };
  };
  return {
    isLoading,
    fetch,
  };
};
