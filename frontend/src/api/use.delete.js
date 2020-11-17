import { axios } from "./";
import { useState } from "preact/hooks";

export const useDelete = (url) => {
  const [isLoading, setIsLoading] = useState(false);

  const del = async () => {
    setIsLoading(true);
    let response;
    try {
      response = await axios.delete(url);
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
    delete: del,
  };
};
