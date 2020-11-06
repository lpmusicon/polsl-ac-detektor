import axios from "axios";

import { useState } from "preact/hooks";

export const usePost = (url) => {
  const [isLoading, setIsLoading] = useState(false);

  const post = async (data) => {
    setIsLoading(true);
    let response;
    try {
      response = await axios.post(url, data);
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
    post,
  };
};
